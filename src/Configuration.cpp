#include "Configuration.hpp"

using namespace std;
using json = nlohmann::json;


Configuration::Configuration() :
    RunAsUnixUserID(RUNAS_USER_DEFAULT),
    RunAsUnixGroupID(RUNAS_GROUP_DEFAULT)
{
    DBG(120, "Constructor");

    try {
        std::ifstream ConfigFile(CONFIG_FILE);
        json jsonData = json::parse(ConfigFile);

        RunAsUnixUser = jsonData["server"]["runas"]["user"];
        RunAsUnixGroup = jsonData["server"]["runas"]["group"];

        BasePath = jsonData["global"]["path"]["base"];
        ServerAddress = jsonData["server"]["connection"]["ipv4"]["address"];
        ServerPort = jsonData["server"]["connection"]["ipv4"]["port"];

        Mimetypes = jsonData["server"]["mimetypes"];

        for (const auto& NamespaceItem:jsonData["namespaces"]) {

            NamespaceProps_t NamespaceProps;

            DBG(-1, "NamespaceItem:" << NamespaceItem);

            NamespaceProps.JSONConfig = std::move(NamespaceItem);
            NamespaceProps.FilesystemRef = nullptr;

            Namespaces.emplace(
                NamespaceItem["hostname"], NamespaceProps
            );
        }
    }
    catch(const json::exception& e) {
        ERR("JSON Config processing error:" << e.what());
        std::exit(1);
    }
}

void Configuration::mapStaticFSData(){
    for (auto& [Host, Namespace]: Namespaces) {

        auto FilesysRef = std::make_shared<Filesystem>();

        DBG(120, "Host:" << Host << " Path:" << Namespace.JSONConfig["path"] << " InterpreterCount:" << Namespace.JSONConfig["interpreters"]);

        FilesysRef->Hostname = Host;
        FilesysRef->Path = string(Namespace.JSONConfig["path"]);
        FilesysRef->BasePath = BasePath;
        FilesysRef->Mimetypes = Mimetypes;

        FilesysRef->initFiles();
        FilesysRef->processFileProperties();

        Namespaces[Host].FilesystemRef = FilesysRef;
    }
}

Configuration::~Configuration()
{
    DBG(120, "Destructor");
}
