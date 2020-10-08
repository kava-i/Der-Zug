
function expand(element) {
  var ul = document.getElementById(element);
  var childern = ul.children;
  for (var i=0; i<childern.length; i++) {
    var elem = childern[i];
    var elem_input = elem.getElementsByTagName("input")[0];
    console.log(elem.id, ", ", elem.value); 
    if (elem.style.display == "none" && elem.id=="optional" && elem_input.value == "")
      childern[i].style.display = "block";
    else if(elem.id == "optional" && elem_input.value == "")
      childern[i].style.display = "none";
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

function del_elem(element, num) {
  console.log("Deleting element: ", element, ", number: ", num);
  var ul = document.getElementById(element);
  ul.removeChild(ul.children[num]);

  //Decrease values of elements after deleted elements
  for (var i=num; i<ul.children.length; i++) {
    ChangeIndexOfClick(ul.children[i].getElementsByTagName("span")[0], -1)
  }
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

  //Get "old" "add-elem" span and change onclick function
  ChangeIndexOfClick(document.getElementById(element).children[index].
    getElementsByTagName("span")[1], 1);

  //Create "del-elem" span.
  var sp = document.createElement("span");
  sp.setAttribute("class", "add_elem");
  sp.setAttribute("onclick", "del_elem('" + element + "', " + new_index + ")");
  sp.innerHTML = "-";

  //Add both to list-element. Then add element to list.
  li.appendChild(sp);
  li.appendChild(add_elem);
  document.getElementById(element).appendChild(li);
}
