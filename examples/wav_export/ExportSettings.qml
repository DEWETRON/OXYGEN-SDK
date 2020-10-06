import QtQuick 2.3
import QtQuick.Layouts 1.1
import Oxygen 1.0
import Oxygen.Dialogs 1.0
import Oxygen.Themes 1.0
import Oxygen.Tools 1.0
import Oxygen.Widgets 1.0

Item {
    id: base

    readonly property real preferredHeight: layout.implicitHeight

    readonly property bool settingsValid: true

    property var customProperties

    function applyDefaults()
    {
        base.customProperties.setString("Format", "PCM")
    }

    onCustomPropertiesChanged:
    {
        formatCombobox.currentIndex = formatCombobox.getIndexByValue(base.customProperties.getString("Format"));
    }

    function translateModel(list, context)
    {
        var translatedModel = Qt.createQmlObject('import QtQuick 2.9; ListModel {}', base);
        for (var idx = 0; idx < list.length; idx++)
        {
            translatedModel.append({
                "text":  qsTranslate(context, list[idx]), "value": list[idx]
            });
        }
        return translatedModel;
    }

    GridLayout {
        id: layout
        anchors.fill: parent
        anchors.leftMargin: Theme.smallMargin
        anchors.rightMargin: Theme.smallMargin

        columns: 2

        Label {
            text: qsTranslate("ODK_WAV_EXPORT/", "Format")
        }

        ComboBox
        {
            id: formatCombobox
            model: translateModel(["Float", "PCM"], "ODK_WAV_EXPORT/")

            onActivated: {
                base.customProperties.setString("Format", model.get(currentIndex).value);
            }
        }
    }
}
