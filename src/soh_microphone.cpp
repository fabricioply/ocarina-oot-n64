#include "n64_microphone_c.h"
#include "n64_adapter.h"
#include "real_instrument/mic_controller.h"
#include <atomic>
#include <memory>
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include <ship/window/gui/Gui.h>
#include <ship/window/gui/GuiWindow.h>
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
extern "C" {
#include "global.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
void AudioOcarina_EnableInput(u8 enabled);
void Message_StartOcarina(PlayState* play,u16 action);
}
namespace {
using real_instrument::MicController;
constexpr uint16_t kDialogText=0x71FE;
std::unique_ptr<MicController> controller;
std::atomic<uint16_t> injection{0};
std::atomic<uint32_t> physical{0};
uint16_t previousInjection[2]{}; // guarded by PadMgr's lock
std::atomic<bool> muted{false};
std::atomic<bool> blockCButtons{false}; // true while our Yes/No box is open
uint32_t lastPhysical=0;
double clockSeconds=0;
struct NativeDialog {
    PlayState* owner=nullptr;
    int scene=-1;
    uint16_t action=0;
    real_instrument::SongConfirmation offer;
    size_t candidate=0;
    int decision=-1;
    enum Stage {Closed,Question,Closing,Resuming} stage=Closed;
    int wait=0;
} dialog;
std::shared_ptr<Ship::GuiWindow> micWindow;
bool showSettingsRequested=false;
class MicrophoneWindow final:public Ship::GuiWindow {
public:
    using GuiWindow::GuiWindow;
    void InitElement() override {}
    void UpdateElement() override {}
    void DrawElement() override {
        if(!controller)return;
        bool enabled=controller->Enabled();
        if(ImGui::Checkbox("Ativar microfone",&enabled))controller->SetEnabled(enabled);
        if(ImGui::Button("Atualizar lista"))controller->RefreshDevices();
        auto labels=controller->ChoiceLabels();int current=controller->SelectedChoice();
        const char* preview=current>=0 && static_cast<size_t>(current)<labels.size()?labels[current].c_str():"Padrao do Windows";
        if(ImGui::BeginCombo("Microfone",preview)){
            for(size_t i=0;i<labels.size();++i)
                if(ImGui::Selectable(labels[i].c_str(),current==static_cast<int>(i)))controller->SelectChoice(static_cast<int>(i));
            ImGui::EndCombo();
        }
        auto status=controller->Status();
        ImGui::TextWrapped("Em uso: %s",controller->ActiveDeviceLabel().c_str());
        ImGui::ProgressBar(std::min(1.f,status.level*5.f),ImVec2(-1,0));
        ImGui::Text("Ultima nota: %s",status.lastNote.empty()?"--":status.lastNote.c_str());
        if(!status.error.empty())ImGui::TextWrapped("Estado: %s",status.error.c_str());
        if(!controller->Notice().empty())ImGui::TextWrapped("%s",controller->Notice().c_str());
        ImGui::TextWrapped("Abra a ocarina e termine a frase com uma pausa. Se houver duvida, o jogo pergunta Sim/Nao.");
    }
};
void EnsureController(){
    if(controller)return;
    controller=std::make_unique<MicController>(MicController::FromEnvironment("config/ocarina_microfone_n64.json"));
    micWindow=std::make_shared<MicrophoneWindow>("gWindows.OcarinaMicrophone",false,"Microfone da Ocarina - N64");
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->AddGuiWindow(micWindow);
}
void ClearSession(){
    injection=0;muted=false;blockCButtons=false;physical=0;lastPhysical=0;dialog={};
    if(controller)controller->ResetGame();
}
void OpenQuestion(){
    dialog.stage=NativeDialog::Question;dialog.wait=0;
    blockCButtons=true;injection=0;
    AudioOcarina_EnableInput(false);
    Message_StartTextbox(dialog.owner,kDialogText,nullptr);
    dialog.owner->msgCtx.choiceIndex=0;
    dialog.stage=NativeDialog::Question;dialog.wait=0;
}
void BuildQuestion(uint16_t* id,bool* vanilla){
    if(*id!=kDialogText || dialog.stage!=NativeDialog::Question || dialog.candidate>=dialog.offer.choices.size())return;
    const auto& choice=dialog.offer.choices[dialog.candidate];
    CustomMessage question(choice.id+"?&\x1B%gSim&Nao%w",TEXTBOX_TYPE_BLACK,TEXTBOX_POS_MIDDLE);
    question.AutoFormat();question.LoadIntoFont();*vanilla=false;
}
void TickDialog(real_instrument::GameMicrophone& microphone,uint32_t physicalPressed){
    auto* play=dialog.owner;
    if(dialog.stage==NativeDialog::Question){
        // C buttons are filtered from the native pad while this owner is
        // active. Use the unfiltered edge for C-Up's native confirm role.
        const uint32_t pressed=play->state.input[0].press.button | (physicalPressed&(BTN_CUP|BTN_B));
        const auto action=ocarina_n64::DecideQuestion(
            Message_GetState(&play->msgCtx)==TEXT_STATE_CHOICE,
            play->msgCtx.textId==kDialogText,
            play->msgCtx.msgMode==MSGMODE_NONE || play->msgCtx.msgMode==MSGMODE_TEXT_CLOSING,
            pressed,play->msgCtx.choiceIndex);
        if(action==ocarina_n64::QuestionAction::Release){
            // Another native message took ownership. Do not close or replace it.
            AudioOcarina_EnableInput(true);microphone.Reset();dialog={};blockCButtons=false;return;
        }
        if(action!=ocarina_n64::QuestionAction::Wait){
            SPDLOG_INFO("Ocarina N64 dialog: action={} text={} mode={} pressed={}",
                static_cast<int>(action),play->msgCtx.textId,play->msgCtx.msgMode,pressed);
            dialog.decision=action==ocarina_n64::QuestionAction::Accept?static_cast<int>(dialog.candidate):-1;
            if(action==ocarina_n64::QuestionAction::Retry)dialog.candidate=dialog.offer.choices.size();
            Message_CloseTextbox(play);dialog.stage=NativeDialog::Closing;dialog.wait=0;
        }
    } else if(dialog.stage==NativeDialog::Closing){
        if(play->msgCtx.msgMode==MSGMODE_NONE || ++dialog.wait>90){
            if(dialog.decision<0 && ++dialog.candidate<dialog.offer.choices.size()){
                OpenQuestion();return;
            }
            if(dialog.decision<0)dialog.decision=static_cast<int>(dialog.offer.choices.size());
            Message_StartOcarina(play,dialog.action);
            AudioOcarina_EnableInput(true);dialog.stage=NativeDialog::Resuming;dialog.wait=0;
        }
    } else if(dialog.stage==NativeDialog::Resuming){
        const bool ready=play->msgCtx.msgMode==MSGMODE_OCARINA_PLAYING;
        if(ocarina_n64::CanResumeOcarina(ready,physical.load())){
            microphone.Resolve(dialog.offer.token,dialog.decision);dialog={};blockCButtons=false;
        } else if(!ready && ++dialog.wait>120){
            microphone.Reset();dialog={};
        }
    }
}
void Frame(){
    EnsureController();
    if(showSettingsRequested){controller->RefreshDevices();micWindow->Show();showSettingsRequested=false;}
    const bool settingsOpen=micWindow && micWindow->IsVisible();
    controller->SetMonitoring(settingsOpen);
    auto* microphone=controller->GameInput();
    if(!microphone){
        injection=0;muted=false;blockCButtons=false;
        if(dialog.stage!=NativeDialog::Closed && gPlayState==dialog.owner){
            Message_StartOcarina(dialog.owner,dialog.action);AudioOcarina_EnableInput(true);
        }
        dialog={};return;
    }
    bool loaded=GameInteractor::IsSaveLoaded(true);
    auto* play=gPlayState;
    if(dialog.stage!=NativeDialog::Closed && (!loaded || play!=dialog.owner || play->sceneNum!=dialog.scene)){
        ClearSession();return;
    }
    uint32_t buttons=physical.load();uint32_t pressed=buttons&~lastPhysical;lastPhysical=buttons;
    const bool ownDialog=dialog.stage!=NativeDialog::Closed;
    blockCButtons=ownDialog;
    if(ownDialog)TickDialog(*microphone,pressed);
    blockCButtons=dialog.stage!=NativeDialog::Closed;
    const bool freePlay=loaded && play->msgCtx.msgMode==MSGMODE_OCARINA_PLAYING &&
        ocarina_n64::SupportedAction(play->msgCtx.ocarinaAction);
    auto gui=Ship::Context::GetRawInstance()->GetWindow()->GetGui();
    const ocarina_n64::UiState ui{settingsOpen,gui->GetMenuOrMenubarVisible(),ImGui::GetIO().WantTextInput,
        ImGui::GetIO().WantCaptureMouse,ImGui::GetIO().WantCaptureKeyboard,
        loaded && play->pauseCtx.state!=0,loaded && play->transitionTrigger!=0,loaded && play->csCtx.state!=0};
    const bool blocked=ocarina_n64::MicrophoneUiBlocked(ui);
    ocarina_n64::Context context{loaded,freePlay || dialog.stage!=NativeDialog::Closed,blocked,
        loaded?(static_cast<uint64_t>(static_cast<uint16_t>(play->sceneNum))<<48) ^ reinterpret_cast<uintptr_t>(play):0,
        loaded?gSaveContext.inventory.questItems:0};
    // Trace only gate transitions, without recording PCM or changing the gate.
    const int gate=(loaded?1:0)|(freePlay?2:0)|(blocked?4:0)|(settingsOpen?8:0)|
        (gui->GetMenuOrMenubarVisible()?16:0)|(ImGui::GetIO().WantCaptureKeyboard?32:0)|
        (ImGui::GetIO().WantCaptureMouse?64:0)|(dialog.stage!=NativeDialog::Closed?128:0)|(ImGui::GetIO().WantTextInput?256:0);
    static int lastGate=-1;
    if(gate!=lastGate || (pressed&0x800F)){
        SPDLOG_INFO("Ocarina N64 gate: bits={} msgMode={} action={} pressed={} held={} pause={} transition={} cutscene={}",
            gate,loaded?static_cast<int>(play->msgCtx.msgMode):-1,loaded?static_cast<int>(play->msgCtx.ocarinaAction):-1,
            pressed,buttons,loaded?static_cast<int>(play->pauseCtx.state):-1,
            loaded?static_cast<int>(play->transitionTrigger):-1,loaded?static_cast<int>(play->csCtx.state):-1);
        lastGate=gate;
    }
    // Clock follows native logic updates, not wall time (shader compilation stalls).
    clockSeconds+=static_cast<double>(R_UPDATE_RATE>0?R_UPDATE_RATE:1)/60.;
    auto output=microphone->Tick(ocarina_n64::BuildState(context),!ownDialog && (pressed&0x800F),clockSeconds);
    injection=dialog.stage==NativeDialog::Closed?ocarina_n64::N64Buttons(output.buttons):0;
    muted=output.audioGain==0;
    if(dialog.stage==NativeDialog::Closed && freePlay && !blocked){
        auto offer=microphone->Confirmation();
        if(offer && !offer->choices.empty()){
            dialog.owner=play;dialog.scene=play->sceneNum;dialog.action=play->msgCtx.ocarinaAction;
            dialog.offer=*offer;dialog.candidate=0;dialog.stage=NativeDialog::Question;
            injection=0;OpenQuestion();
        }
    }
}
void Register(){
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateMainStart>(Frame);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>(ClearSession);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnOpenText>(BuildQuestion);
}
static RegisterShipInitFunc init(Register);
}
extern "C" void OcarinaN64_InjectPad(void* raw,int mode){
    auto* input=static_cast<Input*>(raw);if(!input)return;
    const uint32_t original=input[0].cur.button;
    if(mode)physical=original;
    uint16_t next=injection.load();uint16_t& previous=previousInjection[mode?1:0];
    ocarina_n64::PadEdges edges{original,input[0].press.button,input[0].rel.button};
    if(blockCButtons.load())edges=ocarina_n64::StripCButtons(edges);
    auto merged=ocarina_n64::MergeButtons(edges,next,previous);
    input[0].cur.button=merged.current;input[0].press.button=merged.pressed;
    input[0].rel.button=merged.released;previous=next;
}
extern "C" int OcarinaN64_AudioMuted(){return muted.load()?1:0;}
extern "C" void OcarinaN64_ShowSettings(){showSettingsRequested=true;}
