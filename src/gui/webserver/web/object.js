/**
 * @author fux (georgbuechner)
 */

//Expand optional values for text-elements.
function expand(index) {
  var children = document.getElementById("description_" + index).children;
  for (var i=0; i<children.length; i++) {
    var elem = children[i];
    if (elem.style.display == "none" && elem.id=="optional") {
      document.getElementById("text_expand_"+index).setAttribute("class", "fas fa-caret-up");
      elem.style.display = "block";
    }
    else if(elem.style.display != "none" && elem.id == "optional") {
      document.getElementById("text_expand_"+index).setAttribute("class", "fas fa-caret-down");
      elem.style.display = "none";
    }
  }
}

//Delete text element from description.
function del_text(index) {
  //Change index of following elements and delet element.
  var elem = document.getElementById("li_description_" + index);
  ChangeIdOfTextElements(index+1, -1);  
  elem.parentNode.removeChild(elem);

  //Add special "add_text"-button in case last element was deleted.
  if (document.getElementById("description").children.length==0) {
    var div = MyCreateElement("div", {"id":"div_add_text", "class":"expand"});
    var span = MyCreateElement("span",{"title":"Add new element here.","onclick":"add_text(-1)"});
    span.innerHTML = "+";
    div.appendChild(span);
    document.getElementById("description").parentNode.appendChild(div); 
  }
}

//Add a new text-element to description.
function add_text(index) {
  //Change index of following elements, if not last.
  var ul = document.getElementById("description");
  if (index+1 < ul.children.length)
    ChangeIdOfTextElements(index+1, 1);  

  //Add element at given position.
  InsertIntoList(ul, CreateEmptyDescriptionElement(index+1), index);

  //If called with special "add_text"-button, delete this button.
  if (index==-1)
    DelElem("div_add_text");
}

//Delete element (currently any none-description element in a room)
function del_elem(element, index) {
  //Get list and childern, new index and the matching "add_elem"-function.
  var ul = document.getElementById(element)
  var list_elems = ul.children;
  var new_index = parseInt(index) -1;
  var func = "add_elem_list";
  if (element == "exits" || element == "handlers")
    func = "add_elem_map";

  //If last element, add special "add_elem"-button
  if (list_elems.length == 1) {
    var div = MyCreateElement("div", {"id":"div_add_"+element, "class":"expand"});
    div.appendChild(CreateModElemButton(func, element, -1));
    document.getElementById(element).parentNode.appendChild(div);
  }

  //If last element, move "add-elem"-span to prior element and change onclick function
  if (list_elems.length > 1 && list_elems.length-1 == index)
    list_elems[new_index].appendChild(CreateModElemButton(func, element, new_index));

  //Remove element and decrease values of elements after deleted element
  ul.removeChild(list_elems[index]);
  for (var i=index; i<list_elems.length; i++) {
    ChangeIndexOfClick(list_elems[i].getElementsByTagName("span")[0], -1);
    if (i==list_elems.length-1) 
      ChangeIndexOfClick(list_elems[i].getElementsByTagName("span")[1], -1);
  }
}

//Add list-type element (currently any non-description elements in room (no handlers/ exits))
function add_elem_list(element, index) {
  //Create input fields (id, value/ attributes).
  var li = document.createElement("li");
  li.appendChild(CreateInput(element.substr(0, element.length-1)+" id", "", ""));
  li.appendChild(CreateInput("custom attributes", "{}", ""));

  //Delete old "add-elem" button, and add a new one. Also add "del-elem"-button
  if (index==-1)
    DelElem("div_add_"+element);
  else
    DelElem(element, {"index":index, "tag":"span", "num":1});
  li.appendChild(CreateModElemButton("del_elem", element, index+1));
  li.appendChild(CreateModElemButton("add_elem_list", element, index+1));
  document.getElementById(element).appendChild(li);
}

//Add list-type element (currently only handlers and exits).
function add_elem_map(element, index) {
  var placeholders = ["linked_room id", "name", "preposition"];
  if (element == "handlers")
    placeholders = ["function", "command", "priority"];

  //Add input fields matching element-type
  var li = document.createElement("li");
  for (var i = 0; i<3; i++) {
    if (i==2)
      li.appendChild(CreateInput(placeholders[i], "", "width: 4em"));
    else 
      li.appendChild(CreateInput(placeholders[i], "", ""));
  }

  //Delete old "add-elem" button, and add a new one. Also add "del-elem"-button
  if (index == -1) 
    DelElem("div_add_" + element);
  else
    DelElem(element, {"index":index, "tag":"span", "num":1});
  li.appendChild(CreateModElemButton("del_elem", element, index+1));
  li.appendChild(CreateModElemButton("add_elem_map", element, index+1));
  document.getElementById(element).appendChild(li);
}

// ***** ***** SUPPORTING FUNCTIONS ***** ***** //

//Change index given to a onclick function (add value of x to index)
function ChangeIndexOfClick(elem, x) {
  var onclick = elem.getAttribute("onclick");
  var firstDigit = onclick.match(/\d/);
  var new_num = parseInt(firstDigit) + x;
  var pos = onclick.indexOf(firstDigit);
  var new_onclick = onclick.substr(0, pos) + new_num + onclick.substr(pos+1)
  elem.setAttribute("onclick", new_onclick );
}

//Change all set indexes inside a text element (add value of x to index)
function ChangeIdOfTextElements(start, x) {
  ul = document.getElementById("description");
  for (var i=start; i<ul.children.length; i++) {
    var new_num = i+x;
    console.log("Changed ", i, "to: ", new_num);
    ul.children[i].id = "li_description_" + new_num;
    ul.children[i].children[0].id = "description_" + new_num;
    var expand_div = ul.children[i].children[1];
    expand_div.id = "div_text_expand_" + new_num;
    expand_div.children[1].id = "text_expand_" + new_num;
    ChangeIndexOfClick(expand_div.children[0], x);
    ChangeIndexOfClick(expand_div.children[1], x);
    ChangeIndexOfClick(expand_div.children[3], x);
  }
}

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

//Delete and element given by id or "deeper" access (children, tagname...)
function DelElem(id, opts={}) {
  var elem = document.getElementById(id);
  if (opts['index']) elem = elem.children[opts['index']];
  if (opts['tag'] && opts['num']) elem = elem.getElementsByTagName(opts['tag'])[opts['num']];
  elem.parentNode.removeChild(elem);
}

// ***** ***** ELEMENT CREATORS ***** ***** //

//Create element and set additional attributes (class, id, ...).
function MyCreateElement(type, attrs={}) {
  var elem = document.createElement(type);
  for (var key in attrs)
    elem.setAttribute(key, attrs[key]);
  return elem;
}

//Create input field and set attributes (placeholders, value, style).
function CreateInput(placeholder, value, style) {
  var inp = MyCreateElement("input",{"placeholder":placeholder,"value":value,"class":"m_input"});
  if (style != "") 
    inp.style = style;
  return inp;
}

//Create empty description.
function CreateEmptyDescriptionElement(index) {
  var text = MyCreateElement("li", {"id":"li_description_" + index});
  var ul = MyCreateElement("ul", {"id":"description_" + index});

  //Create all labels
  var label_content = ["Speaker", "Text", "Logic", "Updates", "Pre permanent Events",
    "Pre one-time Events", "Post permanent Events", "Post one-time Events"];
  var labels = [];
  for (var i=0; i<8; i++) {
    var l = document.createElement("label");
    l.innerHTML = label_content[i] + ":";
    if (i>3) l.style="width:auto;";
    labels.push(l);
  }
  
  //Create all li elements and add label.
  var lists = [];
  for (var i=0; i<8; i++) {
    var li = document.createElement("li");
    if (i>1) {
      li.style="display:none;";
      li.id = "optional";
    }
    if (i==2 || i==3) li.setAttribute("class", "full_length");
    li.appendChild(labels[i]);
    lists.push(li);
  }

  //Create all input fields.
  lists[0].appendChild(CreateInput("speaker", "", ""));       //Speaker
  ul.appendChild(lists[0]);
  lists[1].appendChild(document.createElement("textarea"));   //Text
  ul.appendChild(lists[1]);
  lists[2].appendChild(CreateInput("logic", "", ""));         //Logic
  ul.appendChild(lists[2]);
  lists[3].appendChild(CreateInput("updates", "{}", ""));       //Updates
  ul.appendChild(lists[3]);
  lists[4].appendChild(CreateEventType("pre_pEvents"));       //Pre_pEvents
  ul.appendChild(lists[4]);
  lists[5].appendChild(CreateEventType("pre_otEvents"));      //Pre_otEvents
  ul.appendChild(lists[5]);
  lists[6].appendChild(CreateEventType("post_pEvents"));      //Post_pEvents
  ul.appendChild(lists[6]);
  lists[7].appendChild(CreateEventType("post_otEvents"));     //Post_otEvents
  ul.appendChild(lists[7]);
  text.appendChild(ul);

  //Create function-bar (add-, delete- and expand-buttons)
  var div = MyCreateElement("div", {"id":"div_text_expand_"+index, "class":"expand"});
  var span1 = MyCreateElement("span", {"title":"Delete this element.", 
      "onclick":"del_text("+index+")"});
  span1.innerHTML = "-";
  div.appendChild(span1);
  var span2 = MyCreateElement("span", {"id":"text_expand_"+index,"title":"Delete this element.",
    "class":"fas fa-caret-down","onclick":"expand("+index+")"});
  div.appendChild(span2);
  var span3 = MyCreateElement("span", {"title":"Add new element here.", 
    "onclick":"add_text("+index+")"});
  span3.innerHTML = "+";
  div.appendChild(span3);
  text.appendChild(div);

  return text;
}

//Create an empty event-field
function CreateEventType(id) {
  var ul = MyCreateElement("ul", {"id":id});
  var li = document.createElement("li");
  li.appendChild(CreateInput("event", ""));
  ul.appendChild(li);
  return ul;
}

//Create a "mod-element"/button (add_elem_list, add_elem_map, del_elem)
function CreateModElemButton(func, element, index) {
  var sp = MyCreateElement("span", {"class":"mod_elem"});
  sp.setAttribute("onclick", func + "('" + element + "', " + index + ")");
  if (func.indexOf("del") != -1) 
    sp.innerHTML = "-";
  else if (func.indexOf("add") != -1) 
    sp.innerHTML = "+";
  return sp;
}
