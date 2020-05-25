import os
import json
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
                "exits" : "list",
                "items" : "list",
                "details" : "list",
                "attacks" : "list",
                "name" : "string",
                "id" : "string"
            }

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
    def selectCategory(self, name):
        #Create new window
        self.inWorld = Tk()
        self.inWorld.title("Game-Designer: " + name)
        self.path = self.path + name + "/jsons"

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
        self.inWorld.mainloop()

    #Loop to select specific json files in category
    def selectFile(self, name):

        #Update path
        self.pathToCategory= self.path + "/" + name;

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
    def selectObject(self, name):

        #Update path and load json
        self.pathToFile = self.pathToCategory + "/" + name
        with open(self.pathToFile) as json_file:
            data = json.load(json_file)

        #Create label  to show user to select object/ show overview of json
        lbl = Label(self.selectionFrame, text = "select object to edit, or show json.")
        lbl.grid(column=0, row=4)

        #List all objects in json
        objects = Combobox(self.selectionFrame)
        objests_list = list()
        for key, value in data.items():
            objects_list.append(key)
        objects["values"] = objext_list
        objects.current(0)
        objects.grid(column=0, row=5)
        
        #Create two buttons
        btn = Button(self.selectionFrame, text="Accept", command=lambda : self.editObject(objects.get()))
        btn.grid(column=1, row=5)
        btn2 = Button(self.selectionFrame, text="Overview", command=self.showJson)
        btn2.grid(column=3, row=5)
        self.inWorld.mainloop()


    ### ### ----- xx  EDITING OBJECT xx ----- ### ####

    #Edit an object
    def editObject(self, name):

        #Destory old widgets
        for widget in self.editFrame.winfo_children():
            widget.destroy()        

        #Create "title"-label, showing objects that will be edited
        lbltxt = name
        if "name" in self.getObject(name):
            lbltxt=self.getObject(name)["name"] 
        lbl=Label(self.editFrame, text="Editing: " + lbltxt)
        lbl.configure(font="Verdana 16 underline") 
        lbl.grid(column=0, row=0, columnspan=2, pady=7) 

        #Loop over attributes and call specific function to present attributes
        #  in a nicely formatted way
        counter = 1
        for key, value in self.getObject(name).items():
            if key not in self.fields or isinstance(value, str):
                self.printString(counter, key, value)  
            elif self.fields[key] == "list_object":
                self.printListObjekt(counter, key, value)
            elif self.fields[key] == "list":
                self.printList(counter, key, value)
            counter += 1
        self.inWorld.mainloop()


    #Print attributes if value is a basic type, that can easily be printed in one line
    def printString(self, counter, key, value):

        #Frame to hold description
        frame = Frame(self.editFrame)
        frame.grid(column=0, row=counter, columnspan=2, sticky="NW")
        
        #Describing label
        lbl=Label(frame, width=15, text=key + ":")
        lbl.grid(column=0, row=0, sticky="W")
        
        #Textfield for editing
        txt = Entry(frame, width=80)
        txt.insert(0, str(value))
        txt.grid(column=1, row=0, columnspan=2)


    #Print attributes if value is a list, with no extra formatting for list elements
    def printList(self, counter, key, value):

        #Frame to hold description
        frame = Frame(self.editFrame)
        frame.grid(column=0, row=counter, columnspan=2, sticky="NW")

        #Describing label
        lbl=Label(frame, width=15,text=key + ":")
        lbl.grid(column=0, row=0, sticky="W")

        #Insert list elements as text fields for editing
        rows=0
        for i in value:
            txt = Entry(frame, width=80)
            txt.insert(0, str(i))
            txt.grid(column=1, row=rows)
            rows = rows + 1

    #Print attributes if value is a list, with extra formatting for list elements
    def printListObjekt(self, counter, key, value):

        #Frame to hold description
        frame = Frame(self.editFrame)
        frame.grid(column=0, row=counter, columnspan=2, sticky="NW")

        #Describing label
        lbl=Label(frame, width=15,text=key + ":")
        lbl.grid(column=0, row=0, sticky="NW")

        #Iterate over list elements
        counter = 0
        for i in value:
            frame2=Frame(frame)
            frame2.grid(column=1, row=counter, pady=5)

            #Iterate over attributes
            counter2=0
            for k, v in i.items():
                nlbl = Label(frame2, width=13,text=k + ":")
                nlbl.grid(column=0, row=counter2, sticky="NW")
            
                #Use scrollbar for text elements (calculate height)
                if k == "text":
                    l= len(str(v)) / 67 + 1
                    txt = scrolledtext.ScrolledText(frame2, width=67, height=l) 
                    txt.insert(INSERT, str(v))
                else:
                    txt = Entry(frame2, width=67)
                    txt.insert(0, str(v))
                txt.grid(column=1, row=counter2, columnspan=2, sticky="W")
                counter2 = counter2 + 1
            counter = counter + 1


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


    #Load a json from disc and return object
    def getObject(self, name):
        with open(self.pathToFile) as json_file:
            data = json.load(json_file) 
        if name not in data: 
            return {}
        return data[name]

designer = GameDesigner()
designer.selectWorld()
