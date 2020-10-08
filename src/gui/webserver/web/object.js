
function expand(element) {
  var ul = document.getElementById(element);
  var children = ul.children;
  for (var i=0; i<children.length; i++) {
    var elem = children[i];
    var elem_input = elem.getElementsByTagName("input")[0];
    console.log(elem.id, ", ", elem.value); 
    if (elem.style.display == "none" && elem.id=="optional" && elem_input.value == "")
      children[i].style.display = "block";
    else if(elem.id == "optional" && elem_input.value == "")
      children[i].style.display = "none";
  }
}

function ChangeIndexOfClick(elem, x) {
  var onclick = elem.getAttribute("onclick") 
  var firstDigit = onclick.match(/\d/);
  var new_num = parseInt(firstDigit) + x;
  var pos = onclick.indexOf(firstDigit);
  var new_onclick = onclick.substr(0, pos) + new_num + onclick.substr(pos+1)
  elem.setAttribute("onclick", new_onclick );
}

function CreateModElemButton(func, element, index) {
  var sp = document.createElement("span");
  sp.setAttribute("class", "mod_elem");
  sp.setAttribute("onclick", func + "('" + element + "', " + index + ")");
  if (func.indexOf("del") != -1) 
    sp.innerHTML = "-";
  else if (func.indexOf("add") != -1) 
    sp.innerHTML = "+";
  return sp;
}

function del_elem(element, index) {
  console.log("Deleting element: ", element, ", number: ", index);
  //Get list and childern
  var ul = document.getElementById(element)
  var list_elems = ul.children;
  
  //If last element, move "add-elem"-span to prior element and change onclick function
  console.log(list_elems.length, ", ", index);
  if (list_elems.length > 1 && list_elems.length-1 == index) {
    var new_index = parseInt(index) -1;
    var func = "add_elem_list";
    if (element == "exits" || element == "handlers")
      func = "add_elem_map";
    list_elems[new_index].appendChild(CreateModElemButton(func, element, new_index));
  }

  //Remove element and decrease values of elements after deleted element
  ul.removeChild(list_elems[index]);
  for (var i=index; i<list_elems.length; i++)
    ChangeIndexOfClick(list_elems[i].getElementsByTagName("span")[0], -1);
}

function add_elem_list(element) {
  var li = document.createElement("li");
  var id = document.createElement("input");
  id.placeholder = element.substr(0, element.length-1) + " id";
  id.style="margin-right:1em";
  var val = document.createElement("input");
  val.value = "{}";
  li.appendChild(id);
  li.appendChild(val);
  document.getElementById(element).appendChild(li);
}

function add_elem_map(element, index) {
  var placeholders = [];
  if (element == "exits")
    placeholders = ["linked_room id", "name", "preposition"];
  else if (element == "handlers")
    placeholders = ["function", "command", "priority"];

  //Add input fields matching element-type
  var li = document.createElement("li");
  for (var i = 0; i<3; i++) {
    var inp = document.createElement("input");
    inp.setAttribute("class", "m_input");
    if (i==2) inp.style="width: 4em;";
    inp.placeholder = placeholders[i];
    li.appendChild(inp);
  }

  //Delete old "add-elem" button, and add a new one. Also add "del-elem"-button
  var old_del_btn=document.getElementById(element).children[index].getElementsByTagName("span")[1];
  old_del_btn.parentNode.removeChild(old_del_btn);
  var new_index = parseInt(index)+1;
  li.appendChild(CreateModElemButton("del_elem", element, new_index));
  li.appendChild(CreateModElemButton("add_elem_map", element, new_index));
  document.getElementById(element).appendChild(li);
}
