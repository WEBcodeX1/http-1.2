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
        if (!ConfigFile.is_open()) {
            ERR("Could not open config file: " << CONFIG_FILE);
            exit(1);
        }
        json jsonData = json::parse(ConfigFile);

        RunAsUnixUser = jsonData["server"]["runas"]["user"];
        RunAsUnixGroup = jsonData["server"]["runas"]["group"];

        BasePath = jsonData["global"]["path"]["base"];
        ServerAddress = jsonData["server"]["connection"]["ipv4"]["address"];
        ServerPort = jsonData["server"]["connection"]["ipv4"]["port"];

        Mimetypes = jsonData["server"]["mimetypes"];

        for (const auto& NamespaceItem:jsonData["namespaces"]) {

            string NamespaceID;
            NamespaceProps_t NamespaceProps;

            DBG(-1, "NamespaceItem:" << NamespaceItem);

            NamespaceProps.FilesystemRef = nullptr;

            NamespaceID = NamespaceItem["hostname"];
            NamespaceProps.PathRel = NamespaceItem["path"];
            NamespaceProps.InterpreterCount = NamespaceItem["interpreters"];

            DBG(-1, "Namespace:" << NamespaceID << " Path:" << NamespaceProps.PathRel << " Interpreters:" << NamespaceProps.InterpreterCount);

            Namespaces.insert(
                NamespacePair_t(NamespaceID, NamespaceProps)
            );
        }
    }
    catch( const char* msg )
    {
    catch( const nlohmann::json::exception& e )
    {
        ERR("Config file parse error:" << e.what());
        exit(1);
    }
}

Configuration::~Configuration()
{
    DBG(120, "Destructor");
}
