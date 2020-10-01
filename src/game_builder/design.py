import os
import json
import subprocess
import signal
import time
import copy
from functools import partial
from tkinter import *
from tkinter.ttk import *
from tkinter import scrolledtext

class GameDesigner:
    def __init__ (self):

        self.backup = dict()
        self.back = list()

        self.breakcrumbs = list()
        self.curName = ""
        self.file = ""
        self.category = ""

        self.window = Tk()
        self.window.title("Game designer")
        self.path = "factory/"
        self.fields = {
                "text" : "list_object",
                "roomDescription" : "list_object",
                "deadDescription" : "list_object",
                "description" : "list_object",
                "options" : "list_object",
                "steps" : "list_object",
                "listener":"list",
                "characters" : "list",
                "exits" : "object",
                "attacks" : "object",
                "items" : "list",
                "details" : "list",
                "quests" : "list",
                "pages" : "list_object"
            }
        desc = [{"speaker":"", "text":""}]
        pages = [{"speaker":"", "text":"", "page":0}]

        options = [{"id":0, "text":"", "to":""}]
        steps = [{"name":"", "id":"", "handler":"", "description":"", "events":"", "info":{"":""}, "updates":"", "_events":{"":""}, "linkedSteps":[], "logic":""}]
        self.attributes = {
            "dialogs": {"id":"", "text":desc, "options":options, "actions":"", "events":"", "function":""},
            "details": {"name":"", "id":"", "description":desc, "look":"", "items":[""], "defaultItems":""},
            "characters": {"name":"", "id":"", "hp":0, "strength":0, "faint":0, "roomDescription":desc, "description":desc, "deadDescription":desc, "items":[""], "defaultDescription":"", "defaultDialog":"","attacks":{"":""}, "dialog":"", "handlers":""},
            "players": {"name":"", "id":"", "room":"", "hp":0, "strength":0, "attacks":{"":""}, "quests":[""]},
            "quests": {"name":"", "id":"", "description":"", "listener":[""], "ep":0, "steps": steps, "active_from_beginning":[""]},
            "items": {"name":"", "id":"", "category":"", "type":"", "attack":"", "value":0, "description":desc, "pages":pages},
            "rooms": {"name": "", "id":"", "description": desc, "entry" : "", "exits": {"":""}, "characters" : [""], "items" : [""], "details": [""], "handlers":""},
            "attacks": {"name":"", "id":"", "description":"", "power":0},
            "texts":{"id": "", "text":desc}
        }

        description = {"speaker":"", "text":"", "pre_otEvents":"", "pre_pEvents":"", "post_otEvents":"", "post_pEvents":"", "deps":"", "updates":"", "logic":""}
        description_pages = {"speaker":"", "text":"", "pre_otEvents":"", "pre_pEvents":"", "post_otEvents":"", "post_pEvents":"", "deps":"", "updates":"", "page":1}
        self.subAttributes = {
            "text" : description,
            "description" : description,
            "roomDescription" : description,
            "deadDescription" : description,
            "pages" : description_pages,
            "options": {"id":0, "text":"", "to":"", "logic":""},
            "steps": steps[0]
            }
        self.initialSubAttributes = {
            "text": {"speaker":"", "text":""},
            "description":{"speaker":"", "text":""},
            "roomDescription":{"speaker":"", "text":""},
            "deadDescription":{"speaker":"", "text":""},
            "pages":{"speaker":"", "text":"", "page":0},
            "options":{"id":0, "text":"", "to":""},
            "steps": steps[0]
        }

        self.jsonType = ["post_pEvents", "post_otEvents", "pre_pEvents", "pre_otEvents", "updates", "deps", "characters", "details", "items", "linkedSteps", "handler", "handlers", "listener", "info", "active_from_beginning"]

        self.helpDesc = {
            "name":"Name shown to the player",
            "id":"unique identifier, usually in lowercase with _/- instead of spaces. For some objects (items, rooms), the id of the room, or playing owning the item is automatically added to the id, please don't do so by yourself.",
            "description":"Either simple description, or embedded into Text-Element (usually shown, when object is examined",
            "output":"Text printed, when attack is executed",
            "attacks":"A list of attacks, referenced by using the id of an attack.",
            "defaultDescription":"Set to load a random description from a set of descriptions. F.e. 'zombies'",
            "defaultDialog":"Set to load a random default dialog from a set of dialogs. F.e. 'person'",
            "roomDescription":"Text printed, when description of the room is shown",
            "dialog":"link to (special) dialog. Usually the dialog in the format [name]_1 f.e. 'taxifahrer_1' is (if it exists) automatically loaded. ('.json' must be ignored)",
            "pre_otEvents":"Events have the form '[(command) (identifierer)]', 'pre'-> before test is printed, 'ot' one time",
            "handlers":"Json format list of objects. Each objects needs: id (name of handler), string/ regex (either matching string are regular expression)"
        }

        self.curWrite = dict()

    def selectWorld(self):
        #First window to select world
        lbl = Label(self.window, text="Select world")
        lbl.grid(column=0, row=0)
        worlds = Combobox(self.window)
        worlds["values"] = ("world1", "world2")
        worlds.current(0)
        worlds.grid(column=0, row=1)

        btn = Button(self.window, text="Accept", command=lambda:self.selectCategory(worlds.get()))
        btn.grid(column=1, row=1)
        self.window.mainloop()

    #Loop to select category (dialogs, rooms, etc.)
    def selectCategory(self, world):
        
        #Create new window
        self.inWorld = Tk()
        self.inWorld.title("Game-Designer: " + world)
        self.path = "factory/" + world + "/jsons"
 
        #Create scrollable canvas
        self.container = Frame(self.inWorld)
        self.container.pack(anchor="nw", fill=BOTH, expand=True)

        self.canvas = Canvas(self.container)
        self.canvas.pack(side="left", fill="both", expand=True)

        self.scrollbar = Scrollbar(self.container, orient="vertical", command=self.canvas.yview)

        self.scrollable_frame = Frame(self.canvas)
        #self.scrollable_frame.pack(fill=X)

        
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )

        self.canvas.create_window((0,0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        self.scrollbar.pack(side="right", fill="y")
        

        #Create Main containers
        self.selectionFrame = Frame(self.scrollable_frame)
        self.editFrame = Frame(self.scrollable_frame)
        self.overviewFrame = Frame(self.scrollable_frame)
        self.helpFrame = Frame(self.scrollable_frame)
        self.selectionFrame.grid(column=0, row=0, sticky="NW", padx=10, pady=15)
        self.editFrame.grid(column=0, row=1, sticky="NW", padx=10, pady=15, columnspan=2)
        self.helpFrame.grid(column=1, row=0, rowspan=4, pady=5, sticky="NW")
        self.overviewFrame.grid(column=2, row=0, rowspan=6, padx=10, sticky="N")

        #Crete a label showing help informations
        self.hlp = scrolledtext.ScrolledText(self.helpFrame, width=37, height=8)
        self.hlp.insert(INSERT, "Hover over label, or button to see help message.")
        self.hlp.grid(column=1, sticky="NW", row=0)
        
        #Create label to show user to select 
        lbl = Label(self.selectionFrame, text="select category to edit")
        lbl.grid(column=0, row=0)

        #List all categories (load from disc)
        category = Combobox(self.selectionFrame)
        category["values"] = self.listFiles(self.path)
        if len(category["values"]) > 0:
            category.current(0) #set the selected item
        category.grid(column=0, row=1)

        self.file = ""
        self.category = category.get()

        #Create button to accept and link to function
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.selectFile(category.get()))
        btn.grid(column=1, row=1)
        btn_run = Button(self.selectionFrame, text="run", command=self.run)
        btn_run.grid(column=2, row=1)
        btn_end = Button(self.selectionFrame, text="end", command=self.end)
        btn_end.grid(column=3, row=1)
        btn_reset = Button(self.selectionFrame, text="reset", command=self.reset)
        btn_reset.grid(column=4, row=1)
        btn_reset.bind("<Enter>", partial(self.on_enter, "Reset to start of programm."))
        btn_reset.bind("<Leave>", self.on_leave)
        btn_back = Button(self.selectionFrame, text="back", command=self.step_back)
        btn_back.grid(column=5, row=1)
        btn_back.bind("<Enter>", partial(self.on_enter, "Erase last editing step."))
        btn_back.bind("<Leave>", self.on_leave)

        self.inWorld.mainloop()

    def on_enter(self, msg, event):
        self.hlp.delete("1.0", END)
        self.hlp.insert(INSERT, msg)

    def on_leave(self, enter):
        self.hlp.delete("1.0", END)
        self.hlp.insert(INSERT, "Hover over label, or button to see help message.")

    #Function to reset all changes in current session
    def reset(self):
        for key, value in self.backup.items():
            print("writing ", key, ": ")
            print(value)
            f = open(key, "w")
            json.dump(value, f)
            f.close()
        self.backup=dict()

        self.reload() 

    #Function to reset only last step
    def step_back(self):
        #If empty, return
        if len(self.back) == 0:
            return

        #Write object to disc 
        path = self.back[-1][0]
        obj = self.back[-1][1]
        print("writing: ", path)
        print(obj)
        f = open(path, "w")
        json.dump(obj, f)
        f.close()
        #Erase object
        del self.back[-1]
        
        self.reload()


    #Loop to select specific json files in category
    def selectFile(self, category):

        #Update path
        self.pathToCategory= self.path + "/" + category;

        #Create label to show user to select file
        lbl = Label(self.selectionFrame, text="select file to edit")
        lbl.grid(column=0, row=2)

        #List all files (load from disc)
        files = Combobox(self.selectionFrame)
        files["values"] = self.listFiles(self.pathToCategory)
        if len(files["values"]) > 0:
            files.current(0)
        files.grid(column=0, row=3)
        
        self.file = files.get()
        self.pathToFile = self.pathToCategory + "/" + self.file
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.selectObject(files.get()))
        btn.grid(column=1, row=3)

        #Button to create new object
        btn1 = Button(self.selectionFrame, text="add", command=self.addNewfile)
        btn1.grid(column=2, row=3)

        #Button to delete object
        btn2 = Button(self.selectionFrame, text="del", command=self.deleteFile)
        btn2.grid(column=3, row=3)

        self.inWorld.mainloop()

    def addNewfile(self):
        popup = Tk() 
        popup.wm_title("!")
        lbl = Label(popup, text="Enter name:")
        lbl.pack()
        lbl.type="lbl" 
        txt = Entry(popup)
        txt.pack()
        txt.type="txt"
        B1 = Button(popup, text="Okay", command = lambda : self.destroy(popup))
        B1.pack()
        popup.mainloop()

    def destroy(self, popup_win):
        new_name = ""
        for widget in popup_win.winfo_children():
            if widget.type=="txt":
                new_name = widget.get()
                break
        self.pathToFile = self.pathToCategory+"/"+new_name+".json"
        popup_win.destroy()
        new_json = dict()
        f = open(self.pathToFile, "w")
        json.dump(new_json, f)
        f.close()
        self.reload()
        print(self.pathToFile)
        


    def deleteFile(self):
        #Backup file
        self.tobackup()
        #Delete and reset path to file
        os.remove(self.pathToFile)
        self.file = ""
        #Reload gui
        self.reload()
        


    #Loop to select object in json file
    def selectObject(self, File):

        #Update path and load json
        self.pathToFile = self.pathToCategory + "/" + File 
        with open(self.pathToFile) as json_file:
            data = json.load(json_file)

        #Create label  to show user to select object/ show overview of json
        lbl = Label(self.selectionFrame, text = "select object to edit, or show json.")
        lbl.grid(column=0, row=4)

        #descList all objects in json
        objects = Combobox(self.selectionFrame)
        objects_list = list()
        for key, value in data.items():
            objects_list.append(key)
        objects_list.sort()
        objects["values"] = objects_list 
        if len(objects["value"]) > 0:
            objects.current(0)
        objects.grid(column=0, row=5)
        self.curObjects = objects
        
        #Accept Button
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.editObject(objects.get()))
        btn.grid(column=1, row=5)

        #Button to show overview
        btn2 = Button(self.selectionFrame, text="Overview", command=self.showJson)
        btn2.grid(column=2, row=5)

        #Button to create new object
        btn3 = Button(self.selectionFrame, text="+", command=lambda : self.editObject(""))
        btn3.grid(column=3, row=5)

        #Button to delete object
        btn4 = Button(self.selectionFrame, text="-", command=lambda : self.deleteObject(objects.get()))
        btn4.grid(column=4, row=5)
        
        self.inWorld.mainloop()


    ### ### ----- xx  EDITING OBJECT xx ----- ### ####

    def deleteObject(self, name):

        #backup and get json to modifie 
        json_object = self.tobackup()

        #modifie object
        del json_object[name]

        #Write modified json
        f = open(self.pathToFile, "w")
        json.dump(json_object, f)
        f.close()

        #Reset stuff
        self.reload()
       
    #Edit an object
    def editObject(self, name):

        self.curName = name

        #Destory old widgets
        for widget in self.editFrame.winfo_children():
            widget.destroy()        

        #Get json of object to edit and add missing attributes
        edit_object = self.getObject(name)
        for key, value in self.attributes[self.getFromPath(-2)].items():
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
            frame = self.addFrame2(self.editFrame, 0, counter, 2)
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
        txt = self.addEntry(frame, 83, value, 1, 0, key, 2)
        self.curObject[key] = txt


    #Print attributes if value is a list, with no extra formatting for list elements
    def printList(self, counter, key, value, frame):

        #Insert list elements as text fields for editing.
        frame.counter=0
        self.curObject[key] = list()
        for elem in value:
            self.addNew_list(frame, key, elem)
            frame.counter += 1

    #Add new list element
    def addNew_list(self, frame, key, elem):
        frame2 = Frame(frame)
        frame2.grid(column=2, row=frame.counter, columnspan=1)

        #Label for numbering
        lbl = self.addLabel2(frame2, 2, "- ", 1, 0)

        #Create entry field and add to current json.
        txt = self.addEntry(frame2, 80, elem, 2, 0, key)
        self.curObject[key].append(txt)

        #Add button to add new value
        btn_new = Button(frame2, width=2, text="+", command=partial(self.addNew_list, frame, key, ""))
        btn_new.grid(column=3, row=0)
        
        #Add tutton to delete value
        btn_del = Button(frame2, width=2, text="-", command=partial(self.delete_list_dict, frame2, key, frame.counter))
        btn_del.grid(column=4, row=0)
 

    #Print attributes in the for of a dictionary (f.e. exitis).
    def printDict(self, counter, key, value, frame):
    
        #Insert object elemts as two text fields for editing (key, value)
        frame.counter=0
        self.curObject[key] = list()
        obj=dict()
        for k, v in value.items():
            self.addNew_dict(frame, key, k, v, obj)
            frame.counter+=1

        #Add entries to json
        self.curObject[key] = obj

        
    def addNew_dict(self, frame, key, k, v, obj):
        
        frame2 = Frame(frame)
        frame2.grid(column=2, row=frame.counter, columnspan=1)

        #Label for numbering 
        lbl = self.addLabel2(frame2, 1, "- ", 1, 0)

        #Add two text fields to edit id and property, then add to object.
        txt = self.addEntry(frame2, 25, k, 2, 0, key)
        txt.type2 = "str"
        txt2 = self.addEntry(frame2, 55, v, 3, 0, key)
        if key == "attacks":
            txt2.type2 = "txt"
        else:
            txt2.type2 = "json"
        obj[k] = (txt, txt2)

        #Add button to add new value
        btn_new = Button(frame2, width=2, text="+", command=partial(self.addNew_dict, frame, key, "", "", obj))
        btn_new.grid(column=4, row=0)
        
        #Add tutton to delete value
        btn_del = Button(frame2, width=2, text="-", command=partial(self.delete_list_dict, frame2, key, k))
        btn_del.grid(column=5, row=0)

    #Delete dict element
    def delete_list_dict(self, frame, key, k):
        for w in frame.grid_slaves():
            w.grid_forget()
        del self.curObject[key][k]


    #Print attributes if value is a list, with extra formatting for list elements
    def printListObjekt(self, counter, key, value, frame):

        #Iterate over list elements
        self.curObject[key] = list()
        frame.counter = 0
        frame2=Frame(frame)
        for elem in value:
            self.addNew(frame, key, elem) 
            frame.counter = frame.counter + 1

                
    #Add a new object to list (either a new already set, or completly new)
    def addNew(self, frame, key, value):

        #Label for numbering
        lbl = self.addLabel(frame, 2, str(frame.counter+1), 1, frame.counter)

        #Initialize frame and new object
        frame2 = Frame(frame)
        frame2.grid(column=2, row=frame.counter, columnspan=1)
        self.curObject[key].append(dict())

        #Iterate over attributes
        frame2.counter=0
        for k, v in value.items():

            #Add label with key and entry field with value. Add new entry in object.
            lbl = self.addLabel(frame2, 13, k, 0, frame2.counter)
            self.curObject[key][frame.counter][k] = self.addEntryOrSrolled(frame2, v, k, 1, frame2.counter)
            frame2.counter += 1

        #Add button to expand unset values if exists
        if self.hasUnsetAttributes(value, key) == True:
            frame2.expand=False
            btn = Button(frame2, width=1, text=">", command=partial(self.expand, key, value, frame2.counter, frame2, frame.counter))
            btn.grid(column=3, row=frame2.counter-1, padx=3)

        #Add button to add new value
        btn_new = Button(frame2, width=2, text="+", command=partial(self.addNew, frame, key, self.initialSubAttributes[key]))
        btn_new.grid(column=4, row=(frame2.counter-1))

        #Add button to delete this entry
        btn_del = Button(frame2, width=2, text="-", command=partial(self.deleteElem, frame2, key, frame.counter))
        btn_del.grid(column=5, row=(frame2.counter-1))


    def deleteElem(self, frame, key, num):
        for w in frame.grid_slaves():
            w.grid_forget()
        del self.curObject[key][num]
 
    #Add value is called when user selects expand button, to also show unset fields.
    def expand(self, key, elem, counter, frame, num):

        #If no already expaned: expand
        if frame.expand == False:
            frame.expand = True

            for k, v in self.subAttributes[key].items():
                #Only add, if not already in object
                if k not in elem:
                    #Add label and entry and add to object.
                    nlbl = self.addLabel(frame, 13, k, 0, counter)
                    self.curObject[key][num][k] = self.addEntry(frame, 67, "", 1, counter, k)

                    #Increase counter.
                    counter = counter + 1
        
        #Remove all expanded fields
        else:
            frame.expand = False
            for w in frame.grid_slaves():
                if int(w.grid_info()["row"]) >= counter:
                    w.grid_forget()

        self.inWorld.mainloop()

    ### ### ----- xx  VIEW JSON xx ----- ### ####

    #Show overview of json with minimal formatting 
    def showJson(self):
        with open(self.pathToFile) as json_file:
            data = json.load(json_file)
        txt=scrolledtext.ScrolledText(self.overviewFrame, width=100, height=50)
        for key, value in data.items():
            txt.insert(INSERT, "_____________________________________\n")
            for k, v in value.items():
                if isinstance(v, list):
                    txt.insert(INSERT, k + ": \n")
                    for e in v:
                        txt.insert(INSERT, "    -- " + str(e) + "\n")
                elif isinstance(v, dict):
                    txt.insert(INSERT, k + ": \n")
                    for k1, v1 in v.items():
                        txt.insert(INSERT, "    " + k1 + ": " + str(v1) + "\n")
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

            #If object is a dictionary (f.e. exits)
            if isinstance(value, dict):
                obj=dict()
                for k, v in value.items():
                    if self.getAsType(v[0]) != "":
                        obj[self.getAsType(v[0])] = self.getAsType(v[1])
                self.curObject[key] = obj
                print(obj)
                if len(obj) == 0: 
                   delete.append(key)  

            #If object is of list-type (f.e. list of item/ characters, but also text 
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

                    #Elements that are simple lists (f.e. lists of exits, items, etc.)
                    else:
                        if self.isDefault(self.myGet(elem)) == False:
                            newList.append(self.getAsType(elem))

                #Check if element is empty
                if len(newList) > 0:
                    self.curObject[key] = newList
                else:
                    delete.append(key)
                    self.curObject[key] = list()
            
            #Normal attribute
            elif value.type == "txt":
                otp = self.getAsType(value)  
                if self.isDefault(otp) == False:
                    self.curObject[key] = self.getAsType(value)
                else:
                    delete.append(key)
                    self.curObject[key] = ""
                       
        for key in delete:
            del self.curObject[key]
        print(self.curObject) 

        #backup and get json
        json_object = self.tobackup()

        #modifie object
        json_object[self.curObject["id"]] = self.curObject

        #Write modified json
        f = open(self.pathToFile, "w")
        json.dump(json_object, f)
        f.close()

        self.reload()
        

     
    #Comnmand to run textadventure
    def run(self):
        self.prozess = subprocess.Popen("make run", stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        time.sleep(3)
        subprocess.run(["firefox", "~/Documents/programming/JanGeschenk/web/index.html"])
        
    def end(self):
        os.killpg(os.getpgid(self.prozess.pid), signal.SIGTERM)
    
    ### ### ----- xx HELPING FUNCTIONS xx ----- ### ###

    #Add json to backup
    def tobackup(self):
        #Load json
        f = open(self.pathToFile, "r")
        json_object = json.load(f)
        f.close()
        #backup to reset to start of programm
        if self.pathToFile not in self.backup:
            print("Added to backup: ", self.pathToFile)
            print(json_object)
            self.backup[self.pathToFile] = copy.copy(json_object) 
        #backup to reset last step
        back = list()
        back.append(self.pathToFile)
        back.append(copy.copy(json_object))
        self.back.append(back)
        
        return json_object

    #Reload
    def reload(self): 
        for widget in self.editFrame.winfo_children():
            widget.destroy()        
        if self.curName != "" and self.getObject(self.curName) != {}:
            self.editObject(self.curName)
        if self.file != "":
            self.selectObject(self.file)
        self.selectFile(self.category)

    #Load a json from disc and return object
    def getObject(self, name):
        with open(self.pathToFile) as json_file:
            data = json.load(json_file) 
        if name not in data: 
            return {}
        return data[name]

    def getFromPath(self, elem):
        print(self.pathToFile)
        x = self.pathToFile.split("/")
        print(x)
        print(x[elem])
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
        if elem.type == "txt":
            val = elem.get()
        elif elem.type == "stxt":
            val = elem.get("1.0", END)
        if val.endswith("\n"):
            return val[:-1]
        else:
            return val

    def isDefault(self, elem):
        if isinstance(elem, list) and len(elem) == 0:
            return True
        if isinstance(elem, str) and (len(elem) == 0 or elem == "\n"):
            return True
        if isinstance(elem, int) and elem == 0:
            return True
        return False

    def getType2(self, key, value):
        r = "str"
        if isinstance(value, int):
            r = "int"
        elif key in self.jsonType:
            r = "json"
        print(key, r)
        return r 

    def getAsType(self, elem):
        if elem.type2 == "int":
            return int(self.myGet(elem))
        elif elem.type2 == "json":
            value = self.myGet(elem)
            value = value.replace("\'", "\"")
            if value == "":
                return ""
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
        msg = "No help yet, if you'd like a help msg, add an issue to github"
        if txt in self.helpDesc:
            msg = self.helpDesc[txt]
        lbl.bind("<Enter>", partial(self.on_enter, msg))
        lbl.bind("<Leave>", self.on_leave)
        return lbl

    def addLabel2(self, frame, w, txt, c, r, cs=1):
        lbl = Label(frame, width=w, text=txt)
        lbl.grid(column=c, row=r, columnspan=cs, sticky="NW")
        lbl.value = txt 
        lbl.type="desc_label"
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

    def addFrame2(self, frame, c, r, cs):
        frame = Frame(self.editFrame)
        frame.grid(column=c, row=r, columnspan=2, sticky="NW", pady=3)
        frame.type="frame"
        return frame

    def listFiles(self, path):
        files = list()
        for filename in os.listdir(path):
            if (filename.find("default") == -1):
                files.append(filename)
        files.sort()
        return files 


designer = GameDesigner()
designer.selectWorld()
