import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "pages" as Pages
import "singletons"

ApplicationWindow {
    id: window
    visible: true
    Material.theme: Theme.isDark ? Material.Dark : Material.Light
    Material.accent: Theme.primaryColor
    font.family: Theme.fontFamily

    title: "{{projectName}}"


    Pages.MainPage{
        anchors.fill: parent
        
        Text{
            anchors.centerIn: parent
            text: "LiveTextify: Voice to Text to Insight"
        }
    }

    Component.onCompleted: SoundEffects.initialize();
}
