/**
 * @author fux (georgbuechner)
*/

global_error = false;

const ALLOW_EMPTY_LISTS = ["listeners"];

// ***** ***** GENERATE JSONS ***** ***** //

//Generate json of an object.
function GenerateJson(element) {
  //Get list of elements and create empty json.
  var json = new Object();
  var object_list = document.getElementById(element).children;

  //Iterate over all elements and create json depending on "custom"-attribute.
  for (var i=0; i<object_list.length; i++) {
    //Simple input fields (f.e. name, id, ...).
    if (object_list[i].hasAttribute("custom") == false)
      AddJsons(json, CreateObject(object_list[i]));
    //Number of inputfields, not assamble in ul. With extra id fields
    else if (object_list[i].getAttribute("custom") == "just_object")
      json[object_list[i].id] = CreateObject(object_list[i]);
    //List of objects (f.e. items, handlers, descriptions...).
    else if (object_list[i].getAttribute("custom") == "list")
      AddJsons(json, CreateList(object_list[i]));
    //Dictionary of objects (f.e. exits, attacks, ...).
    else if (object_list[i].getAttribute("custom") == "map") 
      AddJsons(json, CreateMap(object_list[i]));
    else {
      console.log( "Unkown attribute in 'GenerateJson': ", object_list[i].id, object_list[i].getAttribute("custom"));
    }
  }
  return json;
}

function IsEmpty(elem) {
  if (elem.value == "") 
    return true;
  if (elem.hasAttribute("custom") && elem.getAttribute("custom") == "json"
    && (elem.value == "{}" || elem.value == "[]"))
    return true;
  if (elem.hasAttribute("custom") && elem.getAttribute("custom") == "int" && elem.value == 0)
    return true;
	return false;
}

//Parse a list of inputs to a json list.
function CreateList(elem) {

  //Create empty list, get list of elements and array of all fields.
  var ul = elem.getElementsByTagName("ul")[0];
  var elems = ul.children;
  var list = [];

  //Iterate over all elements and create specific json to add to list
  for (var i=0; i<elems.length; i++) {
    //Skip empty fields.
    var empty_fields = 0;
    for (var j=0; j<GetValueFields(elems[i]).length; j++) {
      if (IsEmpty(GetValueFields(elems[i])[j])) {
        console.log("empty!")
        empty_fields++;
      }
      else 
        console.log("not empty!")
    }
    if (empty_fields == GetValueFields(elems[i]).length) {
      console.log("All empty: ", elems[i]);
      continue;
    }

    //Simple string
    if (elems[i].hasAttribute("custom") == false) {
      list.push(GetAsType(GetValueFields(elems[i])[0]));
    }
    //Parse a list in a list (f.e. items, characters, details, ...).
    if (elems[i].getAttribute("custom") == "list") {
      var json = [];
      var inputs = GetValueFields(elems[i]);
      for (var j=0; j<inputs.length; j++)
        json.push(GetAsType(inputs[j]));
      list.push(json);
    }
    //Parse an object in a list (f.e. handlers, ...).
    else if (elems[i].getAttribute("custom") == "object") 
      list.push(CreateObject(elems[i]));
    else if (elems[i].getAttribute("custom") == "get:object") {
      list.push(JSON.parse(elems[i].getElementsByTagName("input")[0].value));
    }
    //Parse an object whcih input fields are inside another ul (f.e. descriptions).
    else if (elems[i].getAttribute("custom") == "list_object") {
      var id = elems[i].getElementsByTagName("ul")[0].id;
      console.log(id);
      var json = GenerateJson(id);
      list.push(json);
    }
    else
      console.log("Unkown attribute in 'CreateList': ", elems[i].getAttribute("custom"));
  }

  //Create json "{ id:[...] }" and return 
  var json = new Object();
  if (list.length > 0 || ALLOW_EMPTY_LISTS.indexOf(ul.id) >= 0) {
    json[ul.id] = list;
  }
  return json;
}

//Parse a list of inputs to a json map.
function CreateMap(elem) {

  //Create empty map, get list of elements and array of all fields.
  var ul = elem.getElementsByTagName("ul")[0];
  var elems = ul.children;
  var map = new Object();
  
  //Itarate over all elements and create specific json to add to map
  for (var i=0; i<elems.length; i++) {
    console.log(elems[i].getAttribute("custom"));
    //Skip empty fields.
    if (GetValueFields(elems[i])[0].value == "")
      continue;
    //Parse an key, object pair ("{id:{}}") (f.e. exits).
    if (elems[i].getAttribute("custom") == "key_object") {
      var json = CreateObject(elems[i]);
      map[json.id] = json;
      delete map[json.id].id;
    }
    //Parse a simple key, value pair, where value is a string (json, or int).
    else if (elems[i].getAttribute("custom") == "pair") {
      map[GetAsType(GetValueFields(elems[i])[0])] = GetAsType(GetValueFields(elems[i])[1]);
    }
    else if (elems[i].getAttribute("custom") == "key_list") {
      var json = [];
      var value_fields = GetValueFields(elems[i]);
      for (var j=1; j<=elems.length; j++) 
        json.push(GetAsType(value_fields[j]));
      map[GetAsType(GetValueFields(elems[i])[0])] = json;
    }
    else if (elems[i].getAttribute("custom") == "key_value") {
      map[GetValueFields(elems[i])[0].getAttribute("id")] = GetAsType(GetValueFields(elems[i])[0]);
    }
    else
      console.log("Unkown attribute in 'CreateMap': ", elems[i].getAttribute("custom"));
  }
  
  //Create json "{ id:{...} }" and return 
  var json = new Object();
  if (Object.keys(map).length > 0)
    json[ul.id] = map;
  return json;
}

//Iterate over input fields and create a json object.
function CreateObject(elem) {
  var json = new Object();
  var inputs = GetValueFields(elem);
  for (var i=0; i<inputs.length; i++) {
    var val = inputs[i].value;
		console.log("has_required: ", inputs[i], inputs[i].hasAttribute("required"));
    if ((val == "" || (inputs[i].getAttribute("custom")=="json" && val=="{}")) && !inputs[i].hasAttribute("required"))
      continue;
    json[inputs[i].id] = GetAsType(inputs[i]);
  }
  return json;
}

//Get value of an input fields as a specific type (string, json, int, bool).
function GetAsType(elem) {
  try {
    elem.style.border="none";
		if (IsEmpty(elem) && elem.hasAttribute("required")) {
    	throw(elem.value + " is empty but required.");
		}
    if (elem.type === "checkbox") {
      return (elem.checked) ? 1 : 0;
    }
    else if (elem.hasAttribute("custom") == false)
      return elem.value;
    else if (elem.getAttribute("custom") == "int") {
      var is_number = /^-?\d+$/.test(elem.value);
      if (is_number == false)
        throw(elem.value + " is not a number.");
      return parseInt(elem.value);
    }
    else if (elem.getAttribute("custom") == "float") {
      var is_number = /^\d+(\.\d+)?$/.test(elem.value);
      if (is_number == false)
        throw(elem.value + " is not a number.");
      return parseFloat(elem.value);
    }
    else if (elem.getAttribute("custom") == "str_int") {
      var is_number = /^\d+$/.test(elem.value);
      if (is_number == false)
        throw(elem.value + " is not a number.");
      return elem.value;
    }
    else if (elem.getAttribute("custom") == "json")
      return JSON.parse(elem.value);
    else if (elem.getAttribute("custom") == "bool" && (elem.value == "yes" || elem.value == "true")) 
      return 1;
    else if (elem.getAttribute("custom") == "bool" && (elem.value == "no" || elem.value == "false")) 
      return 0;
    else
      throw("Problem parsing: ", elem.id, elem.getAttribute("custom"), elem.value);
  }
  catch(err) {
    console.log(err);
    global_error = true;
    elem.style.border="1px solid red";
    return "";
  }
}

//get all input fields (input + textarea)
function GetValueFields(elem) {
  var inputs = [];
  inputs.push(...elem.getElementsByTagName("input"));
  inputs.push(...elem.getElementsByTagName("textarea"));
  inputs.push(...elem.getElementsByTagName("select"));
  return inputs;
}

//Concanate two jsons
function AddJsons(a, b) {
  Object.keys(b).forEach(function(key) { a[key] = b[key];});
}

// ***** ***** OPEN/ CLOSE MODALS, WRITE JSON ***** ***** //

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal = document.getElementById("modal_write");
  if (event.target == modal)
    modal.style.display = "none";
}

//Close modal, when close-span is clicked (works for both moduls)
function CloseWriteModul() {
  document.getElementById("modal_write").style.display = "none";
}

//Open write modal.
function OpenWriteModal(name) {
  global_error = false;
  document.getElementById("display_json").innerHTML = "";

  var json = GenerateJson("object");
  if (global_error == true) {
    alert("You have an error in your document.");
  }
  else {
    document.getElementById("modal_write").style.display = "block";
    document.getElementById("check_msg").innerHTML = "Are you sure you want to override the "
      + " current room (" + name + ") with following json:";
    document.getElementById("display_json").innerHTML = JSON.stringify(json, null, 4);
    document.getElementById("check_msg").elem_json = json;
  }
}

//Write elemenet into json
function WriteElem(direct=false, force=false) {
  console.log("Writing json");
  //Send request
  var request = new Object;
  request.json = GenerateJson("object"); 
  request.path = window.location.pathname;
  request.direct = direct;
  request.force = force;
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/write_object");
  xhttp.send(JSON.stringify(request));

  //Function to handle request 
  xhttp.onload = function(event){
    //If request fails, display message to user.
    console.log("Response: ", this.responseText);
    if (xhttp.status != 200) {
      if (this.responseText == "9") {
        document.getElementById("modal_write").style.display = "none";
        document.getElementById("modal_log").style.display = "inline-block";
        document.getElementById("check_msg_log").style.color = "red";
        document.getElementById("check_msg_log").innerHTML = 
          "Error when trying to run game after writing object. No changes made.";
        document.getElementById("get_log").style.display = "none";
        document.getElementById("force_write").style.display = "inline-block";
        get_log('write');
      }
      else
        alert("Problem writing json!");
    }
    //Display success message to user.
    else {
      sessionStorage.setItem("notification", "Changes written to disc.");
      window.location = window.location;
    }
  }
}

// ***** ***** DELETE/ ADD/ EXPAND/ ELEMENTS ***** ***** //

//Deletes a given element.
function del_elem(element, index, obj) {
  var ul = document.getElementById(element);
  if (obj !== undefined)
    ul = obj.parentElement.parentElement;
  var li_old = ul.children[index];


  //If element is only element in list, empty values, instead of deleteing.
  if (index == 0 && ul.children.length==1)
    EmptyValues(li_old);

  //Delete element and change index of all the following elements in list.
  else {
    ul.removeChild(li_old);
    for (var i=index; i<ul.children.length; i++) 
      ChangeIndex(ul.children[i], i);
  }
}


//Adds a new element to any sort of lists
function add_elem(element, index, obj) {
  //Get old element and copy to new element
  var ul = document.getElementById(element);
  if (obj !== undefined)
    ul = obj.parentElement.parentElement;
  var li_old = ul.children[ul.children.length-1];
  var li_new = li_old.cloneNode(true); 

  //Empty values and change index
  EmptyValues(li_new);
  ChangeIndex(li_new, index+1);

  //Change index of all following elements in list.
  for (var i=index+1; i<ul.children.length; i++)
    ChangeIndex(ul.children[i], i+1);
  
  //Insert element at given position.
  InsertIntoList(ul, li_new, index);
}

//Expand optional values for text-elements.
function expand(element, index) {

  //Crate element ids suiting index value (set or not set).
  var id_parent = element;
  if (!(index === "")) id_parent += "_"+index;
  var id_expand_div = element + "_text_expand";
  if (!(index === "")) id_expand_div += "_"+index;

  //Iterate over all elements and check if they are "optional".
  var children = document.getElementById(id_parent).children;
  for (var i=0; i<children.length; i++) {
    var elem = children[i];
    if (elem.style.display == "none" && elem.id=="optional") {
      document.getElementById(id_expand_div).setAttribute("class", "fas fa-caret-up");
      elem.style.display = "block";
    }
    else if(elem.style.display != "none" && elem.id == "optional") {
      document.getElementById(id_expand_div).setAttribute("class", "fas fa-caret-down");
      elem.style.display = "none";
    }
  }
}


// ***** ***** SUPPORTING FUNCTIONS ***** ***** //

//Insert element at a given position.
function InsertIntoList(list, elem, pos) {
  //if list is empty, simply add element and end function
  if (list.children.length == 0) {
    list.appendChild(elem);
    return;
  }
  //Copy children and empty children lest.
  var children = [];
  for (var i=0; i<list.children.length; i++)
    children.push(list.children[i]);
  list.children = [];
  //Add children and new element
  for (var i=0; i<children.length; i++) {
    list.appendChild(children[i]);
    if (i == pos) 
      list.appendChild(elem);
  }
}
//Empties all values of input- and textarea-fields
function EmptyValues(element) {
  //If anaother list, call "EmptyValues" for each element
  if (element.getElementsByTagName("ul").length != 0) {
    var ul = element.getElementsByTagName("ul")[0];
    for (var i=0; i<ul.children.length; i++)
      EmptyValues(ul.children[i]);
  }
  //Empty all input-fields 
  var list_inputs = element.getElementsByTagName("input")
  for (var i=0; i<list_inputs.length; i++)
    list_inputs[i].value = "";
  //Empty all textarea-fields
  var list_inputs = element.getElementsByTagName("textarea")
  for (var i=0; i<list_inputs.length; i++)
    list_inputs[i].value = "";
}

//Changing the index of all attributes needing index-change in element
function ChangeIndex(element, index) {

  //Element with additional list inside element. (descriptions)
  if (element.getElementsByTagName("ul").length != 0) {
    ChangeIndexOfAttribute(element, "id", index);
    ChangeIndexOfAttribute(element.children[0], "id", index);
    var div = element.children[1];
    ChangeIndexOfAttribute(div, "id", index);
    var list_span = div.getElementsByTagName("span");
    for (var i=0; i<list_span.length; i++) {
      ChangeIndex(div, index);
    }
  }
  //No extra list inside element.
  else {
    var list_span = element.getElementsByTagName("span");
    for (var i=0; i<list_span.length; i++) {
      ChangeIndexOfAttribute(list_span[i], "onclick", index);
      ChangeIndexOfAttribute(list_span[i], "id", index);
    }
  }
}

//Changing index of a particular attribute of a given element.
function ChangeIndexOfAttribute(elem, attribute, new_index) {
  var elem_attr = elem.getAttribute(attribute);
  if (elem_attr == null) return;
  var firstDigit = elem_attr.match(/\d/);
  var pos = elem_attr.indexOf(firstDigit);
  var new_attribute = elem_attr.substr(0, pos) + new_index + elem_attr.substr(pos+1)
  console.log(attribute, new_attribute);
  elem.setAttribute(attribute, new_attribute);
}

// ***** ***** ELEMENT CREATORS ***** ***** //

//Create element and set additional attributes (class, id, ...).
function MyCreateElement(type, attrs={}) {
  var elem = document.createElement(type);
  for (var key in attrs)
    elem.setAttribute(key, attrs[key]);
  return elem;
}

// **** new editing **** // 

function DeleteItem(type, cur) { 
  const ul = cur.parentNode;  
  // If last element, replace with default.
  if (ul.children.length === 1) {
    AddItem(type, cur);
  }
  // Remove item
  ul.removeChild(cur);
}

function AddItem(type, cur) { 
  // Get default and change display to block
  const default_item = document.getElementById(type + "_default");
  const new_item = default_item.cloneNode(true)
  new_item.style.display = "block";
  // Insert new item
  cur.parentNode.insertBefore(new_item, cur.nextSibling);
}

function EditItem(type, parent_elem) {
  // Get json and edit-dialog-element
  const json = parent_elem.getElementsByTagName("input")[0].value;
  let dialog = document.getElementById("edit_dialog"); 
  // set type and index in dialog for later saving
  dialog["parent_elem"] = parent_elem;
  // Open modal
  dialog.showModal();
	console.log("Sending json: ", json);

  // Send request for content:
  fetch("/api/edit-template/"+type, {
    method: "POST",
    headers: {"Content-Type": "application/json"},
    body: json
  })
  .then((response) => response.text())
  .then((text) => { 
    document.getElementById("edit_content").innerHTML = text;
		// Init handlers and/ or event_attributes: 
		[...dialog.querySelectorAll("select")].forEach(inp => {
			if (inp.id == "handler") 
				SetHandlerArgs(inp.value);
		})
  });
}

function SaveItem() {
  // get dialog and type and index from dialog:
  let dialog = document.getElementById("edit_dialog"); 
  const parent_elem = dialog["parent_elem"];
  console.log("Got parent_elem: ", parent_elem);
  // Generate json-object
	global_error = false;
  document.getElementById("display_json").innerHTML = "";
  const json = CreateObject(dialog);
  if (global_error == true) {
    alert("You have an error in your document.");
		return;
  }

  console.log("Got object: ", json);
  // Check default and generate abstract representation: 
  // Set json-object
  parent_elem.getElementsByTagName("input")[0].value = JSON.stringify(json);
  parent_elem.getElementsByTagName("a")[0].title = JSON.stringify(json);
  // Set abstract representation
  if (parent_elem.parentNode.id == "listeners")
    parent_elem.getElementsByTagName("a")[0].innerHTML= 
      `${json['id']}: ${json['cmd']} ${get(json, 'target', '')}` 
      + ` → ${json['handler']}(${get(json, 'event_attributes', '')})`;
  dialog.close();
}

function SetHandlerArgs(value) {
	const dialog = document.getElementById("edit_dialog");
	const availible = JSON.parse(document.getElementById("fuzzy_finder_div").getAttribute("__handler_args"));
	const form = document.getElementById("handler_args_content");
	form.innerHTML = "";
	const cur_attributes = dialog.querySelector("#event_attributes").value.split("|");
	let arguments_field = dialog.querySelector("#event_attributes");
	// If arguments are specified:
	if (value in availible) {
		arguments_field.setAttribute("required", "required");
		arguments_field.setAttribute("readonly", "readonly");
		arguments_field.setAttribute("onclick", "OpenHandlerArgsDialog()");
		let i = 0;
		availible[value].forEach(item => {
			const el = document.createElement("input");
			// el.className = "integr_elements";
			el.placeholder = item;
			el.id = item;
			el.name = item;
			el.localize = false;
			// el.setAttribute("localize", "localize");
			if (item == "int") {
				el.type = "number";
			}
			if (cur_attributes.length > i) {
				el.value = cur_attributes[i];
				i++;
			}
			form.appendChild(el);
		});
	}
	// Otherwise make input field to normal input field again.
	else {
		arguments_field.removeAttribute("required");
		arguments_field.removeAttribute("readonly");
		arguments_field.removeAttribute("onclick");
	}
}

function SetEventDialog(cur_cmd, cur_attributes=[]) {
	const availible = JSON.parse(document.getElementById("fuzzy_finder_div").getAttribute("__event_args"));
	const form = document.getElementById("event_content");
	form.innerHTML = "";
	document.getElementById("cmd").value = cur_cmd;
	// If arguments are specified, add inputs matching the specified inputs
	if (cur_cmd in availible) {
		console.log(cur_cmd, " is in ", availible);
		let i = 0;
		availible[cur_cmd].forEach(item => {
			const el = document.createElement("input");
			// el.className = "integr_elements";
			el.placeholder = item;
			el.id = item;
			el.name = item;
			el.localize = false;
			if (item == "int") {
				el.type = "number";
			}
			if (cur_attributes.length > i) {
				el.value = cur_attributes[i];
				i++;
			}
			form.appendChild(el);
		});
	}
	// Otherwise add one single input
	else {
		console.log(cur_cmd, " is not in ", availible);
		const el = document.createElement("input");
		// el.className = "integr_elements";
		el.placeholder = "event attributes sperated by |";
		el.id = "event_attributes";
		el.name = "event_attributes";
		el.value = cur_attributes.join("|");
		form.appendChild(el);
	}
}

function SaveHandlerArgs() {
	console.log("SaveHandlerArgs");
	const dialog = document.getElementById("edit_dialog");
	const form = document.getElementById('handler_args_content')
	const formData = new FormData(form)
	let event_attributes = "";
	for (const [key, value] of formData)
		event_attributes += value + "|";
	dialog.querySelector("#event_attributes").value = event_attributes.slice(0, -1); // 
	CloseDialog("handler_args_dialog");
}

function SaveEvent() {
	console.log("SaveEventArgs");

	// Get command
	const cmd = document.getElementById("cmd").value;
	// Get event arguments
	const form = document.getElementById('event_content')
	const formData = new FormData(form)
	let event_str = cmd + " ";
	for (const [key, value] of formData)
		event_str += value + "|";

	const dialog = document.getElementById("event_dialog");
	console.log(dialog, dialog.edit_elem);
	let elem = dialog.edit_elem;
	elem.value = event_str.slice(0, -1); // 
	CloseDialog("event_dialog");
}

function get(object, prop, defaultValue) {
  if (object.hasOwnProperty(prop)) {
    return object[prop];
  } else {
    return defaultValue;
  }
}

function OpenHandlerArgsDialog() {
  document.getElementById("handler_args_dialog").showModal();
}

function OpenEventDialog(event_str, elem) {
	const cur_cmd = event_str.split(" ")[0];
	const cur_attributes = (event_str.split(" ").length > 1) ? event_str.split(" ")[1].split("|") : [];
	SetEventDialog(cur_cmd, cur_attributes);
	const dialog = document.getElementById("event_dialog");
	dialog.edit_elem = elem;
	dialog.showModal();
}

function CloseDialog(name) {
  document.getElementById(name).close();
}
