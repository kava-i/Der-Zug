
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

function del_elem(element, num) {
  console.log("Deleting element: ", element, ", number: ", num);
  var ul = document.getElementById(element);
  ul.removeChild(ul.children[num]);

  //Decrease values of elements after deleted elements
  for (var i=num; i<ul.children.length; i++) {
    var elem_del = ul.children[i].getElementsByTagName("span")[0];
    var onclick = elem_del.getAttribute("onclick") 
    var firstDigit = onclick.match(/\d/);
    var new_num = parseInt(firstDigit)-1;
    var pos = onclick.indexOf(firstDigit);
    var new_onclick = onclick.substr(0, pos) + new_num + onclick.substr(pos+1)
    elem_del.setAttribute("onclick", new_onclick );
  }
}

function add_elem1(element) {
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

function add_elem2(element, index) {
  var placeholders = [];
  if (element == "exits")
    placeholders = ["linked_room id", "name", "preposition"];
  else if (element == "handlers")
    placeholders = ["function", "command", "priority"];

  var li = document.createElement("li");
  for (var i = 0; i<3; i++) {
    var inp = document.createElement("input");
    if (i!=2) inp.style="margin-right: 1em;";
    if (i==2) inp.style="width: 4em;";
    inp.placeholder = placeholders[i];
    li.appendChild(inp);
  }
  var sp = document.createElement("span");
  sp.setAttribute("class", "del_elem");
  index+=1;
  sp.setAttribute("onclick", "del_elem('element', index)");
  sp.innerHTML = "-";
  sp.style="margin-left: 0.5em;";
  li.appendChild(sp);
  document.getElementById(element).appendChild(li);
}
