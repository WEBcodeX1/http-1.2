#include "Configuration.hpp"

using namespace std;
using json = nlohmann::json;


Configuration::Configuration() :
    RunAsUnixUserID(RUNAS_USER_DEFAULT),
    RunAsUnixGroupID(RUNAS_GROUP_DEFAULT)
{
    DBG(120, "Constructor");

    try {
        ifstream ConfigFile(CONFIG_FILE);
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

            Namespaces.insert(
                NamespacePair_t(NamespaceItem["hostname"], NamespaceProps)
            );
        }
    }
    catch(const json::exception& e) {
        ERR("JSON Config processing error:" << e.what());
        exit(1);
    }
}

Configuration::~Configuration()
{
    DBG(120, "Destructor");
}
