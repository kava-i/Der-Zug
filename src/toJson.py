import yaml, json
import os

folders = ["rooms", "items", "dialogs", "players", "attacks"]

for folder in folders: 
    for filename in os.listdir("factory/"+folder):
        path = "factory/"+folder+"/"+filename
        newPath = "factory/jsons/"+folder+"/"+filename.replace(".yaml", ".json")
        print(path)
        with open(path, 'r') as yaml_in, open(newPath, "w") as json_out:
            yaml_object = yaml.safe_load(yaml_in) # yaml_object will be a list or a dict
            json.dump(yaml_object, json_out)


