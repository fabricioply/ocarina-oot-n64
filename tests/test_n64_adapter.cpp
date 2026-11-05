#include "n64_adapter.h"
#include "test_support.h"
using namespace ocarina_n64;
int main(){
    Context c{true,true,false,123,0x3FFC0};
    auto state=BuildState(c);CHECK(state.canPlay);CHECK(state.context==123);
    for(const auto& song:state.songs)CHECK(song.learned);
    const uint16_t time[]={1,0x8000,4,1,0x8000,4};
    const uint16_t lullaby[]={2,8,1,2,8,1};
    for(size_t i=0;i<6;++i){CHECK(N64Buttons(state.songs[5].buttons[i])==time[i]);CHECK(N64Buttons(state.songs[0].buttons[i])==lullaby[i]);}
    c.questItems=1U<<14;state=BuildState(c);CHECK(state.songs[8].learned);CHECK(!state.songs[9].learned);
    c.playing=false;CHECK(!BuildState(c).canPlay);c.playing=true;
    c.freeOcarina=false;CHECK(!BuildState(c).canPlay);c.freeOcarina=true;
    c.uiBlocked=true;CHECK(!BuildState(c).canPlay);
    // Regression from the live game: closed menus, ocarina playing,
    // WantCaptureMouse=true (gate bits 71). Hovering the viewport is normal.
    c={true,true,false,123,0x3FFC0};
    UiState ui;ui.mouseCaptured=true;
    CHECK(!MicrophoneUiBlocked(ui));
    ui.keyboardCaptured=true;CHECK(!MicrophoneUiBlocked(ui));
    c.uiBlocked=MicrophoneUiBlocked(ui);CHECK(BuildState(c).canPlay);
    ui.settingsOpen=true;CHECK(MicrophoneUiBlocked(ui));ui.settingsOpen=false;
    ui.menuOpen=true;CHECK(MicrophoneUiBlocked(ui));ui.menuOpen=false;
    ui.textEditing=true;CHECK(MicrophoneUiBlocked(ui));ui.textEditing=false;
    ui.paused=true;CHECK(MicrophoneUiBlocked(ui));ui.paused=false;
    ui.transition=true;CHECK(MicrophoneUiBlocked(ui));ui.transition=false;
    ui.cutscene=true;CHECK(MicrophoneUiBlocked(ui));
    CHECK(N64Buttons(0x40)==0);CHECK(N64Buttons(0)==0);
    for(uint16_t action:{1,0x30,0x1C,0x24,0x27})CHECK(SupportedAction(action));
    for(uint16_t action:{0,2,0xD,0xF,0x1A,0x28,0x2C,0x2E,0x2F,0x31,0xFFFF})CHECK(!SupportedAction(action));
    auto pad=MergeButtons({0x4000,0x4000,0},0x8000,0);
    CHECK(pad.current==0xC000);CHECK(pad.pressed==0xC000);CHECK(pad.released==0);
    pad=MergeButtons({},0x8000,0x8000);CHECK(pad.pressed==0);CHECK(pad.current==0x8000);
    pad=MergeButtons({},0,0x8000);CHECK(pad.released==0x8000);CHECK(!pad.current);
    pad=MergeButtons({0x8000,0,0},0,0x8000);CHECK(!pad.released);CHECK(pad.current==0x8000);
    // Native Message_ShouldAdvance accepts C-Up as well as A. The module
    // must not remain in Question after the native box has consumed C-Up.
    CHECK(DecideQuestion(true,true,false,8,0)==QuestionAction::Accept);
    CHECK(DecideQuestion(true,true,false,8,1)==QuestionAction::Next);
    CHECK(DecideQuestion(true,true,false,0x8000,0)==QuestionAction::Accept);
    CHECK(DecideQuestion(true,true,false,0x4000,0)==QuestionAction::Retry);
    CHECK(DecideQuestion(false,true,true,0,0)==QuestionAction::Retry);
    CHECK(DecideQuestion(false,false,false,0,0)==QuestionAction::Release);
    CHECK(DecideQuestion(false,true,false,8,0)==QuestionAction::Wait);
    CHECK(DecideQuestion(true,true,false,0,0)==QuestionAction::Wait); // held button, no new edge
    CHECK(DecideQuestion(true,true,false,4,0)==QuestionAction::Wait);
    auto stripped=StripCButtons({0x1000C00F,0x1000C00F,0x1000C00F});
    CHECK(stripped.current==0x1000C000);CHECK(stripped.pressed==0x1000C000);CHECK(stripped.released==0x1000C000);
    CHECK(CanResumeOcarina(true,0));CHECK(!CanResumeOcarina(false,0));
    for(uint32_t held:{0x8000U,0x4000U,1U,2U,4U,8U})CHECK(!CanResumeOcarina(true,held));
}
