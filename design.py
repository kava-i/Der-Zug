import os
import json
import subprocess
import signal
import time
from functools import partial
from tkinter import *
from tkinter.ttk import *
from tkinter import scrolledtext

class GameDesigner:
    def __init__ (self):
        self.window = Tk()
        self.window.title("Game designer")
        self.path = "src/factory/"
        self.fields = {
                "text" : "list_object",
                "roomDescription" : "list_object",
                "description" : "list_object",
                "options" : "list_object",
                "steps" : "list_object",
                "characters" : "list",
                "exits" : "object",
                "items" : "list",
                "details" : "list",
                "attacks" : "list",
            }
        desc = [{"speaker":"", "text":""}]
        options = [{"id":0, "text":"", "to":""}]
        steps = [{"name":"", "id":"", "handler":"", "description":""}]
        self.attributes = {
            "dialogs": {"id":"", "text":desc, "options":options},
            "details": {"name":"", "id":"", "description":desc, "look":"", "items":[""], "defaultItems":""},
            "characters": {"name":"", "id":"", "hp":0, "strength":0, "roomDescription":desc, "description":desc, "defaultDescription":"", "defaultDialog":"","attacks":[""], "dialog":""},
            "players": {"name":"", "id":"", "room":"", "hp":0, "strength":0, "attacks":[""]},
            "quests": {"name":"", "id":"", "description":"", "ep":0, "steps": steps},
            "items": {"name":"", "id":"", "category":"", "type":"", "attack":"", "value":0, "description":desc},
            "rooms": {"name": "", "id":"", "description": desc, "entry" : "", "exits": {"":""}, "characters" : [""], "items" : [""], "details": [""]},
            "attacks": {"name":"", "id":"", "description":"", "power":0}
        }

        description = {"speaker":"", "text":"", "pre_otEvents":"", "pre_pEvents":"", "post_otEvents":"", "post_pEvents":"", "deps":"", "updates":""}
        self.subAttributes = {
            "text" : description,
            "description" : description,
            "roomDescription" : description,
            "options": {"id":0, "text":"", "to":"", "deps":""},
            "steps": steps[0]
            }

        self.jsonType = ["post_pEvents", "post_otEvents", "pre_pEvents", "pre_otEvents", "updates", "deps", "characters", "details", "items"]

        self.curWrite = dict()

    def selectWorld(self):
        #First window to select world
        lbl = Label(self.window, text="Select world")
        lbl.grid(column=0, row=0)
        worlds = Combobox(self.window)
        worlds["values"] = ("world1", "world2")
        worlds.current(0)
        worlds.grid(column=0, row=1)

        btn = Button(self.window, text="Accept", command=lambda:self.selectCategory( worlds.get()))
        btn.grid(column=1, row=1)
        self.window.mainloop()

    #Loop to select category (dialogs, rooms, etc.)
    def selectCategory(self, world):
        #Create new window
        self.inWorld = Tk()
        self.inWorld.title("Game-Designer: " + world)
        self.path = "src/factory/" + world + "/jsons"

        #Create Main containers
        self.selectionFrame = Frame(self.inWorld)
        self.editFrame = Frame(self.inWorld)
        self.overviewFrame = Frame(self.inWorld)
        self.selectionFrame.grid(column=0, row=0, sticky="NW", padx=10, pady=15)
        self.editFrame.grid(column=0, row=1, sticky="NW", padx=10, pady=15)
        self.overviewFrame.grid(column=1, row=0, rowspan=6, padx=10, sticky="N")
        
        #Create label to show user to select 
        lbl = Label(self.selectionFrame, text="select category to edit")
        lbl.grid(column=0, row=0)

        #List all categories (load from disc)
        category = Combobox(self.selectionFrame)
        category_list = list()
        for filename in os.listdir(self.path):
            if (filename.find("default") == -1):
                category_list.append(filename)
        category["values"] = category_list 
        category.current(0) #set the selected item
        category.grid(column=0, row=1)

        #Create button to accept and link to function
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.selectFile(category.get()))
        btn.grid(column=1, row=1)
        btn_run = Button(self.selectionFrame, text="run", command=self.run)
        btn_run.grid(column=2, row=1)
        btn_end = Button(self.selectionFrame, text="end", command=self.end)
        btn_end.grid(column=3, row=1)

        self.inWorld.mainloop()

    #Loop to select specific json files in category
    def selectFile(self, category):

        #Update path
        self.pathToCategory= self.path + "/" + category;

        #Create label to show user to select file
        lbl = Label(self.selectionFrame, text="select file to edit")
        lbl.grid(column=0, row=2)

        #List all files (load from disc)
        files = Combobox(self.selectionFrame)
        files_list = list()  
        for filename in os.listdir(self.pathToCategory):
            files_list.append(filename)
        files["values"] = files_list 
        files.current(0)
        files.grid(column=0, row=3)
        
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.selectObject(files.get()))
        btn.grid(column=1, row=3)
        self.inWorld.mainloop()
    

    #Loop to select object in json file
    def selectObject(self, File):

        #Update path and load json
        self.pathToFile = self.pathToCategory + "/" + File 
        with open(self.pathToFile) as json_file:
            data = json.load(json_file)

        #Create label  to show user to select object/ show overview of json
        lbl = Label(self.selectionFrame, text = "select object to edit, or show json.")
        lbl.grid(column=0, row=4)

        #List all objects in json
        objects = Combobox(self.selectionFrame)
        objects_list = list()
        for key, value in data.items():
            objects_list.append(key)
        objects["values"] = objects_list 
        objects.current(0)
        objects.grid(column=0, row=5)
        
        #Create threebuttons
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.editObject(objects.get()))
        btn.grid(column=1, row=5)
        btn2 = Button(self.selectionFrame, text="Overview", command=self.showJson)
        btn2.grid(column=2, row=5)
        btn3 = Button(self.selectionFrame, text="New", command=lambda : self.editObject(""))
        btn3.grid(column=3, row=5)
        self.inWorld.mainloop()


    ### ### ----- xx  EDITING OBJECT xx ----- ### ####

    
    #Edit an object
    def editObject(self, name):

        #Destory old widgets
        for widget in self.editFrame.winfo_children():
            widget.destroy()        

        #Get json of object to edit and add missing attributes
        edit_object = self.getObject(name)
        for key, value in self.attributes[self.getFromPath(4)].items():
            if key not in edit_object:
                edit_object[key] = value
        self.curObject = dict()

        #Create "title"-label, showing objects that will be edited
        lbltxt = name
        if "name" in edit_object:
            lbltxt=edit_object["name"] 
        lbl=Label(self.editFrame, text="Editing: " + lbltxt)
        lbl.configure(font="Verdana 16 underline") 
        lbl.grid(column=0, row=0, columnspan=2, pady=7) 
        lbl.value = name
        lbl.type="label"

        #Loop over attributes and call specific function to present attributes
        #  in a nicely formatted way
        counter = 1
        for key, value in edit_object.items():

            #Add new frame and describing label.
            frame = self.addFrame(self.editFrame, 0, counter, 2)
            lbl = self.addLabel(frame, 15, key, 0, 0)

            if key not in self.fields or isinstance(value, str):
                self.printString(counter, key, value, frame)  
            elif self.fields[key] == "list_object":
                self.printListObjekt(counter, key, value, frame)
            elif self.fields[key] == "list":
                self.printList(counter, key, value, frame)
            elif self.fields[key] == "object":
                self.printDict(counter, key, value, frame)
            counter += 1
        
        btn_write = Button(self.editFrame, text="write", command=self.write)
        btn_write.grid(column=0, row=counter, columnspan=2)
        btn_json= Button(self.editFrame, text="json", command=lambda : self.justJson(edit_object))
        btn_json.grid(column=0, row=counter+1, columnspan=2)
        self.inWorld.mainloop()
       
 
    #Print attributes if value is a basic type, that can easily be printed in one line
    def printString(self, counter, key, value, frame):

        #Add entry field and add both to json with current object.
        txt = self.addEntry(frame, 80, value, 1, 0, key, 2)
        self.curObject[key] = txt


    #Print attributes if value is a list, with no extra formatting for list elements
    def printList(self, counter, key, value, frame):

        #Insert list elements as text fields for editing.
        rows=0
        self.curObject[key] = list()
        for elem in value:

            #Create entry field and add to current json.
            txt = self.addEntry(frame, 80, elem, 1, rows, key)
            self.curObject[key].append(txt)
            rows = rows + 1

    #Print attributes in the for of a dictionary (f.e. exitis).
    def printDict(self, counter, key, value, frame):
    
        #Insert object elemts as two text fields for editing (key, value)
        rows=0
        self.curObject[key] = list()
        obj=dict()
        for k, v in value.items():

            #Add two text fields to edit id and property, then add to object.
            txt = self.addEntry(frame, 25, k, 1, rows, key)
            txt.type2 = "str"
            txt2 = self.addEntry(frame, 55, v, 2, rows, key)
            txt2.type2 = "json"
            obj[k] = (txt, txt2)

            rows = rows + 1

        #Add entries to json
        self.curObject[key] = obj

    #Print attributes if value is a list, with extra formatting for list elements
    def printListObjekt(self, counter, key, value, frame):

        #Iterate over list elements
        self.curObject[key] = list()
        frame.counter = 0
        for elem in value:
            self.addNew(frame, key, elem) 
            frame.counter = frame.counter + 1
    
        #Add button to add new value
        btn_new = Button(frame, width=3, text="+", command=lambda : self.addNew(frame, key, self.subAttributes[key]))
        btn_new.grid(column=2, row=0, padx=3, sticky="NW")
        
        
    #Add a new object to list (either a new already set, or completly new)
    def addNew(self, frame, key, value):

        #Initialize frame and new object
        frame2 = self.addFrame(frame, 1, frame.counter, 1)
        obj=dict()

        #Iterate ober attributes
        counter=0
        for k, v in value.items():

            #Add label with key and entry field with value. Add new entry in object.
            lbl = self.addLabel(frame2, 13, k, 0, counter)
            txt = self.addEntryOrSrolled(frame2, v, k, 1, counter)
            obj[k] = txt
            counter = counter + 1

            #Add button to expand unset values if exists
            if self.hasUnsetAttributes(value, key) == True:
                frame2.expand=False
                btn = Button(frame2, width=7, text="expand", command=partial(self.addValue, key, value, counter, frame2))
                btn.grid(column=3, row=counter-1, padx=3)

        #Add to json of current object.
        self.curObject[key].append(obj)
             

    #Add value is called when user selects expand button, to also show unset fields.
    def addValue(self, key, elem, counter, frame):

        #If no already expaned: expand
        if frame.expand == False:

            #Initialize new frame and object
            frame.expand = True
            obj = dict()
            for k, v in self.subAttributes[key].items():

                #Only add, if not already in object
                if k not in elem:

                    #Add label and entry and add to object.
                    nlbl = self.addLabel(frame, 13, k, 0, counter)
                    txt = self.addEntry(frame, 67, v, 1, counter, k)
                    obj[k] = txt

                    #Increase counter.
                    counter = counter + 1

            #Add to json of current object.
            self.curObject[key].append(obj)
        
        #Remove al expanded fields
        else:
            frame.expand = False
            for w in frame.grid_slaves():
                if int(w.grid_info()["row"]) >= counter:
                    w.grid_forget()

    ### ### ----- xx  VIEW JSON xx ----- ### ####

    #Show overview of json with minimal formatting 
    def showJson(self):
        with open(self.pathToFile) as json_file:
            data = json.load(json_file)
        txt=scrolledtext.ScrolledText(self.overviewFrame, width=100, height=50)
        for key, value in data.items():
            txt.insert(INSERT, "_____________________________________\n")
            for k, v in value.items():
                if isinstance(v, list) or isinstance(v, dict): 
                    txt.insert(INSERT, k + ": \n")
                    for e in v:
                        txt.insert(INSERT, "    -- " + str(e) + "\n")
                else:
                    txt.insert(INSERT, k + ": " + str(v) + "\n")
                                
            txt.insert(INSERT, "_____________________________________\n\n")
        txt.grid(column = 6, row=4, columnspan = 6, rowspan=10)
        self.inWorld.mainloop()

    def justJson(self, obj):
        txt=scrolledtext.ScrolledText(self.overviewFrame, width=100, height=50)
        for key, value in obj.items():
            txt.insert(INSERT, str(key) + ": " + str(value) + "\n")
        txt.grid(column = 6, row=4, columnspan = 6, rowspan=10)
        self.inWorld.mainloop()
    
    ### ### ----- xx OTHER FUNCTIONS xx ----- ### ###

    #Create writable json, that write data
    def write(self):

        delete = list()
        #Iterate over created json and change attibutes
        for key, value in self.curObject.items():

            if isinstance(value, dict):
                obj=dict()
                for k, v in value.items():
                    if self.getAsType(v[0]) != "":
                        obj[self.getAsType(v[0])] = self.getAsType(v[1])
                self.curObject[key] = obj
                print(obj)
                if len(obj) == 0: 
                   delete.append(key)  

            elif isinstance(value, list):
                newList = list()
                for elem in value:

                    
                    #Elements that are a list of dicts (f.e. options, texts etc.)
                    if isinstance(elem, dict):
                        obj = dict()
                        for k,v in elem.items():
                            if self.isDefault(self.getAsType(v)) == False:
                                obj[k] = self.getAsType(v)
                        if len(obj) > 0:
                            newList.append(obj)

                    #Elements that are simple lists (f.e. exits, items, characters etc.)
                    else:
                        if self.isDefault(self.myGet(elem)) == False:
                            newList.append(self.getAsType(elem))

                #Check if element is empty
                if len(newList) > 0:
                    self.curObject[key] = newList
                else:
                    delete.append(key)
                    self.curObject[key] = list()
            elif value.type == "txt":
                self.curObject[key] = self.getAsType(value)
                       
        for key in delete:
            del self.curObject[key]
        print(self.curObject) 

        #Load json and change modified object
        f = open(self.pathToFile, "r")
        json_object = json.load(f)
        f.close()
        json_object[self.curObject["id"]] = self.curObject

        #Write modified json
        f = open(self.pathToFile, "w")
        json.dump(json_object, f)
        f.close()
     
    #Comnmand to run textadventure
    def run(self):
        self.prozess = subprocess.Popen("make run", stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        time.sleep(3)
        subprocess.run(["firefox", "~/Documents/programming/JanGeschenk/web/index.html"])
        
    def end(self):
        os.killpg(os.getpgid(self.prozess.pid), signal.SIGTERM)
    
    ### ### ----- xx HELPING FUNCTIONS xx ----- ### ###

    #Load a json from disc and return object
    def getObject(self, name):
        with open(self.pathToFile) as json_file:
            data = json.load(json_file) 
        if name not in data: 
            return {}
        return data[name]

    def getFromPath(self, elem):
        x= self.pathToFile.split("/")
        return x[elem]
    
    def hasUnsetAttributes(self, elem, key):
        for k, v in self.subAttributes[key].items():
            if k not in elem:
                return True
        return False 
    
    def hasAttr(self, elem):
        try:
            t = elem.type
        except:
            return False
        return True

    def myGet(self, elem):
        val = ""
        if elem.type == "txt":
            val = elem.get()
        elif elem.type == "stxt":
            val = elem.get("1.0", END)
        if val.endswith("\n"):
            return val[:-1]
        else:
            return val

    def isDefault(self, elem):
        print("elem: ", str(elem))
        if isinstance(elem, list) and len(elem) == 0:
            return True
        if isinstance(elem, str) and (len(elem) == 0 or elem == "\n"):
            return True
        if isinstance(elem, int) and elem == 0:
            return True
        return False

    def getType2(self, key, value):
        if isinstance(value, int):
            return "int"
        elif key in self.jsonType:
            return "json"
        return "str"

    def getAsType(self, elem):
        if elem.type2 == "int":
            return int(self.myGet(elem))
        elif elem.type2 == "json":
            value = self.myGet(elem)
            value = value.replace("\'", "\"")
            return json.loads(value)
        else:
            val = self.myGet(elem)
            if val.endswith("\n"):
                return val[:-1]
            else:
                return val

    def addLabel(self, frame, w, txt, c, r, cs=1):
        lbl = Label(frame, width=w, text=txt + ":")
        lbl.grid(column=c, row=r, columnspan=cs, sticky="NW")
        lbl.value = txt 
        lbl.type="label"
        return lbl
    
    def addEntry(self, frame, w, value, c, r, key, cs=1):
        txt = Entry(frame, width=w)
        txt.insert(0, str(value))
        txt.grid(column=c, row=r, columnspan=cs)
        txt.type="txt"
        txt.type2 = self.getType2(key, value)
        return txt

    def addEntryOrSrolled(self, frame, value, key, c, r):
        if key == "text":
            h = len(str(value))/67+1
            txt = scrolledtext.ScrolledText(frame, width=67, height=h)
            txt.insert(INSERT, str(value))
            txt.type="stxt"
            txt.type2="str"
        else:
            txt = Entry(frame, width=67)
            txt.insert(0, str(value))
            txt.type="txt"
            txt.type2 = self.getType2(key, value)
        txt.grid(column=c, row=r, sticky="W")
        return txt

    def addFrame(self, frame, c, r, cs):
        frame = Frame(self.editFrame)
        frame.grid(column=c, row=r, columnspan=2, sticky="NW")
        frame.type="frame"
        return frame


designer = GameDesigner()
designer.selectWorld()
