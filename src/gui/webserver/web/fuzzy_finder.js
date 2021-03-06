var paths_ = [];
var parent_object_ = "";
var on_page_objects_ = [];
var cur_object_ = window.location.pathname;
var finder_counter = 0;
var element_counter = 0;
var fuzzy_finder_active = false;
var pages = []
var child_elements_ = (document.getElementsByClassName("side_box").length < 2);
let inps = [];
let cur_input_field_ = "";

window.onload = function() {
  if (sessionStorage.getItem("notification") && sessionStorage.getItem("notification") != "") {
    notify(sessionStorage.getItem("notification"));
    sessionStorage.setItem("notification", "");
  }

  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  paths_ = document.getElementById("fuzzy_finder_div").getAttribute("pages");
  paths_ = JSON.parse(paths_);
  inps_ = paths_;
  if (document.getElementById("fuzzy_finder_div").hasAttribute("page_objects")) {
    page_objects = document.getElementById("fuzzy_finder_div").getAttribute("page_objects");
    page_objects = JSON.parse(page_objects)
    for (var i=0; i<page_objects.length; i++)
      on_page_objects_.push(page_objects[i][1]);
  }
  else 
    console.log("no objects on this page!");

  HighlightElem();
}

let mode_ = "normal"

function checkMatch() {
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  inp = document.getElementById("fuzzy_finder_inp").value;

  // If typeahead, add current user input to list.
  if (mode_ == "typeahead")
    inps_.push(inp);

  var matches = [];

  // Select matching elements.
  for (var i=0; i<inps_.length; i++) {
    var current_page = inps_[i].toLowerCase();
    //If not found, continue
    if (current_page.indexOf(inp) == -1) continue;

    // Calculate score.
    var score = current_page.length/inp.length;
    if (mode_ == "normal") {
      // If current path is a child-path of the current page, increase score.
      if (current_page.indexOf(cur_object_) != -1) score/=2;
      // If last element matches the search word, increase score.
      last_elem = current_page.substr(current_page.lastIndexOf("/"));
      if (last_elem.indexOf(inp) == 0) score/=2;
    }
    matches.push([inps_[i], score])
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
  "a":65, "z":90, "/":191, "left":37, "right":39, "back":8, "strg":17, "1":49, 
  "2":50, "3":51, "0":48
}

document.addEventListener('keydown', function(event) {
  fuzzy_finder_inp = document.getElementById("fuzzy_finder_inp");

  pages = document.getElementById("fuzzy_finder_elems").children;
  
  //ESC -> hide fuzzy finder
  if (event.keyCode == key_codes["esc"]) {
    SelectCloseModals(); 
    SelectCloseFinder();
  }

  // Check if input is ment for an input field which isn't fuzzy finder.
  else if (NonFuzzyFinderInput(event)) {
    return;
  }
  else if (event.keyCode === key_codes["0"]) {
    document.getElementById("try_game_btn").click();
  }
  else if (event.keyCode === key_codes["1"]) {
    ToggleGraph();
  }
  else if (event.keyCode === key_codes["2"]) {
    ToggleNotes();
  }
  else if (event.keyCode === key_codes["3"]) {
    ToggleMarkdown();
  }
  else if (event.keyCode == key_codes["right"]) {
    SelectChildElement(); 
  }
  else if (event.keyCode == key_codes["left"]) {
    SelectParentElement(); 
  }
  // Set modus to delete
  else if (on_page_objects_.length != 0 && fuzzy_finder_inp.value == "" && event.keyCode == key_codes["-"])
    SelectDeleteModus(event);
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
    else
      element_counter = mod(element_counter+1, on_page_objects_.length); 
  }
  // Select matches up.
  else if (event.keyCode == key_codes["up"]) {
    if (fuzzy_finder_active)
      finder_counter = mod(finder_counter-1, pages.length); 
    else
      element_counter = mod(element_counter-1, on_page_objects_.length); 
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
  mode_ = "delete";
  inps_ = on_page_objects_;
  document.getElementById("fuzzy_finder_mode").innerHTML = "delete: ";
  document.getElementById("fuzzy_finder_mode").style.color = "orange";
}

function SelectElement(event) {
  console.log("SelectElement");
  if (fuzzy_finder_active) {
    if (mode_ == "normal")
      window.location = pages[finder_counter].innerHTML;
    else if (mode_ == "typeahead") {
      console.log("Writing ", pages[finder_counter].innerHTML, " to ", cur_input_field_);
      cur_input_field_.value = pages[finder_counter].innerHTML;
      SelectCloseFinder();
    }
    else {
      event.preventDefault();
      OpenDelElemModal('subcategory', pages[finder_counter].innerHTML);
      SelectCloseFinder();
    }
  }
}

function SelectDeleteElement(event) {
  // Check if on fuzzy_finder-input-field. (All other input fields are already checked)
  if (event.target == fuzzy_finder_inp)
    return;
  event.preventDefault();
  OpenDelElemModal('subcategory', on_page_objects_[element_counter]);
}

function SelectChildElement() {
  if (child_elements_) window.location = cur_object_ + "/" + on_page_objects_[element_counter];
}

function SelectParentElement() {
  var cur_depts = (cur_object_.match(/\//g) || []).length;
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
  mode_ = "normal";
  inps_ = paths_; 
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

  if (event.target.hasAttribute("contenteditable"))
    return true;

  for (var index = 0; index < input_fields.length; ++index) {
    if (event.target == input_fields[index] && event.target != fuzzy_finder_inp) {
      // If typeahead, then mode will be changed, otherwiese stop using fuzzy_finder
      if (Typeahead(input_fields[index]))
        return false;
      else
        return true;
    }
  }
  return false;
}

function Typeahead(input_field) {
  // Room type-ahead 
  // - "room": when adding new-player and selecting start-room, 
  // "linked_room id": when adding an exit to a room.
  if (input_field.id == "room" || input_field.placeholder == "linked_room id") {
    inps_ = GetAllX("room");
    mode_ = "typeahead";
    console.log("typeahead modus selected.");
    cur_input_field_ = input_field;
    document.getElementById("fuzzy_finder_mode").innerHTML = "rooms: ";
    document.getElementById("fuzzy_finder_mode").style.color = "blue";
    return true;
  }
  else
    return false;
}

function HighlightElem() {
  for (var i=0; i<pages.length; i++) {
    if (i==finder_counter && mode_ == "normal")
      pages[i].classList.add("fuzzy_finder_focused_green")
    else if (i==finder_counter && mode_ == "delete")
      pages[i].classList.add("fuzzy_finder_focused_orange")
    else if (i==finder_counter && mode_ == "typeahead")
      pages[i].classList.add("fuzzy_finder_focused_blue")
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

function GetAllX(category) {
  results = []
  paths_.forEach(path => {
    let pos = path.indexOf(category)
    if (pos != -1) {
      url_path = window.location.pathname;
      cur_path_without_cur_elem = url_path.substr(0, url_path.lastIndexOf("/"));
      let cur_path = ""
      if (path.indexOf(cur_path_without_cur_elem) != -1)
        cur_path = path.substr(path.lastIndexOf("/")+1);
      else
        cur_path = path.substr(pos + category.length+2);
      results.push(cur_path.replaceAll("/", "."));
    }
  });
  return results;
}
