
function expand(element) {
  var list = document.getElementById(element);
  var childern = list.children;
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

function add_exit(element) {
  var li = document.createElement("li");
  li.appendChild(document.createElement("input"));
  li.appendChild(document.createElement("input"));
  li.appendChild(document.createElement("input"));
  document.getElementById("exits").appendChild(li);
}
