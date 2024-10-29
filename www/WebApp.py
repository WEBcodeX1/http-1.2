#!/usr/bin/python3

import json

class BaseClass:
    def invoke(self, ReqJSON):
        print('"'+ReqJSON+'"')
        Request = json.loads(ReqJSON)
        print(Request["payload"])
        return Request["payload"]


