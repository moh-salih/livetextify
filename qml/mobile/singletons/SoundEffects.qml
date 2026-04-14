pragma Singleton
import QtQuick
import QtMultimedia

QtObject {
    id: root

    // Property to enable/disable all sounds
    property bool muted: false

    // Volume control (0.0 to 1.0)
    property real volume: 0.9

    // Sound effects (binding handles updates automatically)
    property SoundEffect successSound: SoundEffect {
        source: "qrc:/sounds/success.wav"
        volume: root.volume
        muted: root.muted
    }

    property SoundEffect clickSound: SoundEffect {
        property bool initialized: false
        source: "qrc:/sounds/mixkit-fast-double-click-on-mouse-275.wav"
        volume: root.volume
        muted: root.muted
    }


    // Public API
    function playSuccess() { successSound.play() }
    function playClick() { clickSound.play() }


    function initialize(){
        let sounds = [
                clickSound,
                successSound,
            ];

        for(let sound of [clickSound, successSound]){
            sound.volume = 0.0;
            sound.play();
            sound.stop();
            sound.volume = 1.0;
        }

        console.log("All sounds preloaded and ready.");
    }
}
