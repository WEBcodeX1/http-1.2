import json

class BaseClass:
    def invoke(self, ReqJSON):
        Request = json.loads(ReqJSON)
        return Request["payload"]
