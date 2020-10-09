import QtQuick 2.3
import QtQuick.Controls 1.2 as QtControls
import QtQuick.Layouts 1.1
import Oxygen 1.0
import Oxygen.Dialogs 1.0
import Oxygen.Layouts 1.0
import Oxygen.Themes 1.0
import Oxygen.Tools 1.0
import Oxygen.Widgets 1.0


Item
{
    id: root

    property var channels: QObjectTreeModel {}

    property string filename
    property bool csv_valid: false
    readonly property bool settingsValid: filename !== "" && csv_valid

    function queryProperties()
    {
        var props = {}

        props["ODK_REPLAY_SYNC_SCALAR/CSVFile"] = root.filename;

        return props;
    }

    ColumnLayout
    {
        anchors.leftMargin: Theme.smallMargin
        anchors.rightMargin: Theme.smallMargin
        anchors.fill: parent

        spacing: Theme.mediumSpacing

        Button {
            id: fileButton
            text: qsTranslate("ODK_REPLAY_SYNC_SCALAR/AddChannel", "Select CSV file") + Theme.actionEllipsis

            onClicked: {
                fileDialog.open();
            }
        }

        Label
        {
            text: qsTranslate("ODK_REPLAY_SYNC_SCALAR/AddChannel", "Selected CSV file") + ":"
        }

        TextField
        {
            id: idInputField
            Layout.fillWidth: true
            text: root.filename
            readOnly: true
            placeholderText: qsTranslate("ODK_REPLAY_SYNC_SCALAR/AddChannel", "No file selected")
        }

        Label
        {
            text: qsTranslate("ODK_REPLAY_SYNC_SCALAR/AddChannel", "Not a valid CSV file")
            visible: !root.csv_valid
            color: Theme.error
        }

        VerticalSpacer {}

    }

    CustomPluginRequest {
        id: verifyCSVFile
        messageId: 1

        function startCheck(path)
        {
            console.log("Checking validity of file '" + path + "'...");
            var props = plugin.createPropertyList();
            props.setString("filename", path);
            request(props);
        }
        onResponse:
        {
            console.log("Received response: " + value.getBool("valid"));
            root.csv_valid = value.getBool("valid");
        }
    }


    FileDialog
    {
        id: fileDialog
        selectExisting: true
        nameFilters: [qsTranslate("ARINC_PLUGIN/AddChannel", "CSV File") + " (*.csv)"]
        title: qsTranslate("ARINC_PLUGIN/AddChannel", "Open CSV file")
        onAccepted: {
            var path = fileUrl.toString().substr(8);
            root.filename = path;
            root.csv_valid = false;

            verifyCSVFile.startCheck(filename);
        }
        onRejected: {
        }
    }
}
