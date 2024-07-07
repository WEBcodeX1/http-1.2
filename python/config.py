import json

from xml_microparser import microparser


with open('/home/cpruefer/Repos/data-gateway/python/config.xml', encoding='utf-8') as f:

    parser = microparser.Parser(f.read())
    parser.build_serializer()
    parser.process_json()

    root_element = parser.get_root_element()
    config_dict = root_element.get_element_by_element_name('config').get_json_dict()

    print(config_dict)

    config_global = config_dict['config']['global']

    server_run_user = config_global['run']['user']
    server_run_group = config_global['run']['group']

    path_base = config_global['path']['base']

    connection = config_global['connection']

    server_ip = connection['ipv4']['address']
    server_port = connection['ipv4']['port']

    connection_timeout = connection['timeout']

    mimetypes = config_global['server']['mimetypes']['mimetype']
    namespaces = config_dict['config']['namespaces']['namespace']

config = json.dumps(config_dict)
