var paths_ = [];
var parent_object_ = "";
var on_page_objects_ = [];
var cur_object_ = window.location.pathname;
var finder_counter = 0;
var element_counter = 0;
var fuzzy_finder_active = false;
var delete_modus = false;
var pages = []
var child_elements_ = false;

window.onload = function() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  paths_ = document.getElementById("fuzzy_finder_div").getAttribute("pages");
  paths_ = JSON.parse(paths_);
  if (document.getElementById("fuzzy_finder_div").hasAttribute("page_objects")) {
    page_objects = document.getElementById("fuzzy_finder_div").getAttribute("page_objects");
    page_objects = JSON.parse(page_objects)
    for (var i=0; i<page_objects.length; i++)
      on_page_objects_.push(page_objects[i][1]);
  }
  else 
    console.log("no objects on this page!");

  CreateParentObjects();
  CreateChildObjects();
  HighlightElem();
}

function checkMatch() {
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  inp = document.getElementById("fuzzy_finder_inp").value;
  var matches = [];
  var inps = (delete_modus == false) ? paths_ : on_page_objects_

  // Select matching elements.
  for (var i=0; i<inps.length; i++) {
    var current_page = inps[i].toLowerCase();

    //If not found, continue
    if (current_page.indexOf(inp) == -1) continue;

    // Calculate score.
    var score = current_page.length/inp.length;
    if (delete_modus == false) {
      // If current path is a child-path of the current page, increase score.
      if (current_page.indexOf(cur_object_) != -1) 
        score/=2;
      // If last element matches the search word, increase score.
      last_elem = current_page.substr(current_page.lastIndexOf("/"));
      if (last_elem.indexOf(inp) == 0) score/=2;
    }
    matches.push([inps[i], score])
  }

  // Sort and take only first 8 elements
  matches.sort(function(a,b) { return a[1] - b[1]; });
  matches = matches.slice(0,15);

  // Add as elements to dropdown list
  list_match = document.getElementById("fuzzy_finder_elems");
  for (var i=0; i<matches.length; i++) {
    li = document.createElement("li");
    txt = document.createTextNode(matches[i][0]);
    li.appendChild(txt);
    list_match.appendChild(li);
  }

  // Correct finder_counter, in case list got reduced.
  if (finder_counter > list_match.length) 
    finder_counter = list_match.length-1;

  HighlightElem();
}

// Define keycodes for better readability:
key_codes = {"+":61, "-":173, "esc":27, "enter":13, "up":38, "down":40, 
  "a":65, "z":90, "/":191, "left":37, "right":39, "back":8}

document.addEventListener('keydown', function(event) {

  fuzzy_finder_inp = document.getElementById("fuzzy_finder_inp");

  pages = document.getElementById("fuzzy_finder_elems").children;

    // Set modus to delete
  if (on_page_objects_.length != 0 && fuzzy_finder_inp.value == "" && event.keyCode == key_codes["-"])
    SelectDeleteModus(event);
  //ESC -> hide fuzzy finder
  else if (event.keyCode == key_codes["esc"]) {
    SelectCloseFinder();
    SelectCloseModals(); // Regarding this "fuzzy_finder" should be renamed to "key-driven" or something.
  }
  else if (event.keyCode == key_codes["right"]) {
    SelectChildElement(); 
  }
  else if (event.keyCode == key_codes["left"]) {
    SelectParentElement(); 
  }
  // Check if input is ment for an input field which isn't fuzzy finder.
  else if (NonFuzzyFinderInput(event))
    return;
  // Delete cur element.
  else if (event.keyCode == key_codes["back"]) {
    SelectDeleteElement(event);
  }
  // Add element.
  else if (fuzzy_finder_inp.value == "" && event.keyCode == key_codes["+"])
    SelectAddElem(event);
  //Return -> Go to match
  else if (event.keyCode == key_codes["enter"] && pages.length > 0)
    SelectElement(event);
  // Select matches down.
  else if (event.keyCode == key_codes["down"]) {
    if (fuzzy_finder_active)
      finder_counter = mod(finder_counter+1, pages.length); 
    else {
      element_counter = mod(element_counter+1, on_page_objects_.length); 
      CreateChildObjects();
    }
  }
  // Select matches up.
  else if (event.keyCode == key_codes["up"]) {
    if (fuzzy_finder_active)
      finder_counter = mod(finder_counter-1, pages.length); 
    else {
      element_counter = mod(element_counter-1, on_page_objects_.length); 
      CreateChildObjects();
    }
  }
  // Add letter to entry 
  else if ((event.keyCode >= key_codes["a"] && event.keyCode <= key_codes["z"]) 
    || event.keyCode == key_codes["/"])
    SelectAddLetter(event)

  if (fuzzy_finder_inp.value == "")
    document.getElementById("fuzzy_finder_elems").innerHTML = "";
  HighlightElem();

}, true);

function SelectAddElem(event) {
  event.preventDefault();
  OpenAddElemModal();
  document.getElementById("name").value = "";
}

function SelectDeleteModus(event) {
  console.log("Delete modus triggered: ", event.keyCode);
  // Get current element.
  delete_modus = true;
  document.getElementById("fuzzy_finder_mode").innerHTML = "delete: ";
  document.getElementById("fuzzy_finder_mode").style.color = "orange";
}

function SelectElement(event) {
  console.log("SelectElement");
  if (fuzzy_finder_active) {
    if (delete_modus == false)
      window.location = pages[finder_counter].innerHTML;
    else {
      event.preventDefault();
      OpenDelElemModal('subcategory', pages[finder_counter].innerHTML);
      SelectCloseFinder();
    }
  }
}

function SelectDeleteElement(event) {
  // Check if on fuzzy_finder-input-field. (All other input fields are already checked)
  if (event.target == fuzzy_finder_inp) {
    return;
  }
  event.preventDefault();
  OpenDelElemModal('subcategory', on_page_objects_[element_counter]);
}

function SelectChildElement() {
  if (child_elements_)
    window.location = cur_object_ + "/" + on_page_objects_[element_counter];
}

function SelectParentElement() {
  var cur_depts = (cur_object_.match(/\//g) || []).length;
  console.log("Current depth: ", cur_depts);

  if (cur_depts > 3)
    window.location = cur_object_.substr(0, cur_object_.lastIndexOf("/"));
  else
    window.location = "/overview"
}

function SelectCloseFinder() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  document.getElementById("fuzzy_finder_mode").innerHTML = "files: ";
  document.getElementById("fuzzy_finder_mode").style.color = "#1fe921";
  delete_modus = false;
  finder_counter = 0;
  fuzzy_finder_active = false;
}

function SelectCloseModals() {
  //Close every modal.
  modals = document.getElementsByClassName("modal");
  targeted = false;
  for (var i=0; i<modals.length; i++) {
    if (modals[i].style.display === "block" || modals[i].style.display === "inline-block")
      targeted = true;
    modals[i].style.display = "none";
  }
  if (targeted == true) {
    window.location = window.location;
  }
}

function SelectAddLetter(event) {
  fuzzy_finder_active = true;
  checkMatch();
  if (event.target != fuzzy_finder_inp)
    document.getElementById("fuzzy_finder_div").style.display="block";
  fuzzy_finder_inp.focus();
}

function NonFuzzyFinderInput(event, all) {
  var input_fields = [];
  input_fields.push.apply(input_fields, document.getElementsByTagName('input'));
  input_fields.push.apply(input_fields, document.getElementsByTagName('textarea'));
  for (var index = 0; index < input_fields.length; ++index) {
    if (event.target == input_fields[index] && event.target != fuzzy_finder_inp) 
      return true;
  }
  return false;
}

function HighlightElem() {
  for (var i=0; i<pages.length; i++) {
    if (i==finder_counter && delete_modus == false)
      pages[i].classList.add("fuzzy_finder_focused_green")
    else if (i==finder_counter && delete_modus == true)
      pages[i].classList.add("fuzzy_finder_focused_orange")
    else 
      pages[i].classList = [];
  }

  all_elements = document.getElementById("controller");
  if (all_elements == null)
    return; 
  elements = all_elements.getElementsByTagName("tr");
  for (var i=0; i<elements.length; i++) {
    if (i == element_counter)
      elements[i].classList.add("fuzzy_finder_focused_green")
    else 
      elements[i].classList.remove("fuzzy_finder_focused_green")
  }
}

function mod(n, m) {
  return ((n % m) + m) % m;
}

function CreateParentObjects() {
  var cur_depts = (cur_object_.match(/\//g) || []).length;
  paths_.forEach(path => {
    var path_deps = (path.match(/\//g) || []).length;
    var path_parent = path.substr(0, path.lastIndexOf("/"));
    if (cur_depts == path_deps && cur_object_.indexOf(path_parent) == 0) {
      var path_name = path.substr(path.lastIndexOf("/")+1);
      var li = document.createElement("li");
      if (cur_object_.indexOf(path) == 0) {
        li.classList.add("fuzzy_finder_focused_green");
        parent_object_ = path;
      }
      li.setAttribute("path", path);
      li.innerHTML = path_name;
      document.getElementById("parent_paths").appendChild(li);
    }
  })
}

function CreateChildObjects() {
  if (document.getElementsByClassName("side_box").length < 2) {
    child_elements_ = false;
    return;
  }
  child_elements_ = true
  document.getElementById("child_paths").innerHTML = "";
  document.getElementById("object_json").innerHTML = "";
  var cur_depts = (cur_object_.match(/\//g) || []).length;
  var cur_elem_path = cur_object_ + "/" + on_page_objects_[element_counter];
  paths_.forEach(path => {
    var path_deps = (path.match(/\//g) || []).length;
    if (cur_depts+2 == path_deps && path.indexOf(cur_elem_path) == 0) {
      var path_name = path.substr(path.lastIndexOf("/")+1);
      var li = document.createElement("li");
      li.setAttribute("path", path);
      li.innerHTML = path_name;
      document.getElementById("child_paths").appendChild(li);
    }
  });
  if (document.getElementById("child_paths").children.length == 0) {
    console.log("Requesting object json for: ", cur_object_ + "/" + on_page_objects_[element_counter]);
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/api/get_object_json/" + cur_object_ + "/" + on_page_objects_[element_counter]);
    xhttp.send();
    xhttp.onload = function(event){
      //If request fails, display message to user.
      if (xhttp.status == 200) {
        var json = JSON.parse(this.responseText);   
        document.getElementById("object_json").innerHTML = JSON.stringify(json, null, 2);
      }
    }
  }
}

function ShowSideBox(x) {
  var id = "side_box_" + x;
  if (document.getElementById(id).style.display === "none") {
    console.log(x, " visible")
    document.getElementById(id).style.display = "block";
  }
  else {
    console.log(x, " unvisible")
    document.getElementById(id).style.display = "none";
  }
}
