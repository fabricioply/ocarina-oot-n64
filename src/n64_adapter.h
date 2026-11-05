#pragma once
#include "real_instrument/game_input.h"
#include "real_instrument/song_catalog.h"
namespace ocarina_n64 {
struct Context {
    bool playing=false,freeOcarina=false,uiBlocked=false;
    uint64_t scene=0;
    uint32_t questItems=0;
};
struct UiState {
    bool settingsOpen=false,menuOpen=false,textEditing=false;
    bool mouseCaptured=false,keyboardCaptured=false;
    bool paused=false,transition=false,cutscene=false;
};
inline bool MicrophoneUiBlocked(const UiState& ui){
    // ImGui also captures the game viewport. Capture flags alone do not
    // identify an open menu; only explicit UI and text editing block input.
    return ui.settingsOpen || ui.menuOpen || ui.textEditing ||
        ui.paused || ui.transition || ui.cutscene;
}
// Match native Message_ShouldAdvance ownership without letting C-Up also
// reach Navi/the instrument while our question is open.
enum class QuestionAction {Wait,Accept,Next,Retry,Release};
inline QuestionAction DecideQuestion(bool choiceReady,bool ownerTextPresent,bool closed,
    uint32_t pressed,uint8_t selected){
    if(closed)return QuestionAction::Retry;
    if(!ownerTextPresent)return QuestionAction::Release;
    if(pressed&0x4000U)return QuestionAction::Retry; // B cancels even while opening
    if(choiceReady && (pressed&(0x8000U|0x8U)))
        return selected==0?QuestionAction::Accept:QuestionAction::Next;
    return QuestionAction::Wait;
}
inline bool CanResumeOcarina(bool ready,uint32_t physicalHeld){
    return ready && (physicalHeld&0xC00FU)==0; // release A, B and all C buttons
}
struct PadEdges {uint32_t current=0,pressed=0,released=0;};
inline PadEdges MergeButtons(PadEdges pad,uint32_t next,uint32_t previous){
    pad.pressed|=next&~previous&~pad.current;
    pad.released|=previous&~next&~pad.current;
    pad.current|=next;return pad;
}
// While our native Yes/No box is open the real C buttons must not reach the game:
// C-Up is Navi's/first-person button and C-Left/Down/Right are ocarina notes, and
// any of them can start another message or action on top of the question and
// leave the textbox stuck. The dialog owner reads the raw C-Up edge as
// confirmation; A/B and the stick stay available to the native box.
constexpr uint32_t kCButtons=0x000F;
inline PadEdges StripCButtons(PadEdges pad){
    pad.current&=~kCButtons;pad.pressed&=~kCButtons;pad.released&=~kCButtons;return pad;
}
inline bool SupportedAction(uint16_t action){return action==1 || action==0x30 || (action>=0x1C && action<=0x27);}
// The copied recognition core uses these five logical note masks. They are
// translated here; no 3DS address or button mask reaches the N64 game.
inline real_instrument::OcarinaGameState BuildState(const Context& context) {
    real_instrument::OcarinaGameState result;
    if(!context.playing || !context.freeOcarina || context.uiBlocked)return result;
    result.canPlay=true;result.context=context.scene;
    constexpr uint32_t logical[]={0x200,0x100,0x800,0x400,1};
    const auto& catalog=real_instrument::NativeSongCatalog();
    for(size_t song=0;song<catalog.size();++song){
        const auto& definition=catalog[song];
        // OoT's quest song bits are 6..17; the verified catalog stores
        // corresponding item indices 90..101 in its canonical table.
        result.songs[song].learned=(context.questItems&(1U<<(definition.flagItem-84)))!=0;
        for(uint32_t note=0;note<definition.length;++note)
            result.songs[song].buttons.push_back(logical[definition.notes[note]]);
    }
    return result;
}
inline uint16_t N64Buttons(uint32_t logical) {
    if(logical&~0xF01U)return 0;
    uint16_t result=0;
    // D4=A, F4=C-down, A4=C-right, B4=C-left, D5=C-up.
    if(logical&0x200)result|=0x8000;
    if(logical&0x100)result|=4;
    if(logical&0x800)result|=1;
    if(logical&0x400)result|=2;
    if(logical&1)result|=8;
    return result;
}
}
