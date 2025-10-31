pragma Singleton

import QtQuick

QtObject {
    property string foreground: "#000000"
    property string background: "#ffffff"
    property string surface: "#f9f9f9"
    property string selected: "#eaeaea"
    property string stroke: "#e0e0e0"
    property string accent: "#3889F2"

    function withAlpha(col, alpha) {
        var hex = col.replace(/^#/, "")
        var r = parseInt(hex.substr(0,2), 16) / 255
        var g = parseInt(hex.substr(2,2), 16) / 255
        var b = parseInt(hex.substr(4,2), 16) / 255
        return Qt.rgba(r, g, b, alpha)
    }
}
