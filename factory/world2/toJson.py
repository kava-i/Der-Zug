import yaml, json
import os

folders = ["rooms", "items", "characters", "details", "dialogs", "players", "attacks", "quests", "defaultDescriptions", "defaultDialogs"]

for folder in folders: 
    for filename in os.listdir(folder):
        path = folder+"/"+filename
        newPath = "jsons/"+folder+"/"+filename.replace(".yaml", ".json")
        print(path)
        with open(path, 'r') as yaml_in, open(newPath, "w") as json_out:
            yaml_object = yaml.safe_load(yaml_in) # yaml_object will be a list or a dict
            json.dump(yaml_object, json_out)

