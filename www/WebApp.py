import json

class WebApp:
    def invoke(self, ReqJSON):
        Request = json.loads(ReqJSON)
        return str(Request["payload"])
