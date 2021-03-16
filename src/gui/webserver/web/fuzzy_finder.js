var paths = []
var objects = []

window.onload = function() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  paths = document.getElementById("fuzzy_finder_div").getAttribute("pages");
  paths = JSON.parse(paths);
  if (document.getElementById("fuzzy_finder_div").hasAttribute("page_objects")) {
    page_objects = document.getElementById("fuzzy_finder_div").getAttribute("page_objects");
    page_objects = JSON.parse(page_objects)
    for (var i=0; i<page_objects.length; i++)
      objects.push(page_objects[i][1]);
  }
  else 
    console.log("no objects on this page!");
}

function checkMatch() {
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  inp = document.getElementById("fuzzy_finder_inp").value;
  var matches = [];
  var inps = (delete_modus == false) ? paths : objects

  // Select matching elements.
  for (var i=0; i<inps.length; i++) {
    var current_page = inps[i].toLowerCase();

    //If not found, continue
    if (current_page.indexOf(inp) == -1) continue;

    // Calculate score.
    var score = current_page.length/inp.length;
    if (delete_modus == false) {
      // If current path is a child-path of the current page, increase score.
      if (current_page.indexOf(window.location.pathname) != -1) 
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
  "a":65, "z":90, "/":191}

var finder_counter = 0;
var delete_modus = false;
document.addEventListener('keydown', function(event) {

  fuzzy_finder_inp = document.getElementById("fuzzy_finder_inp");
  pages = document.getElementById("fuzzy_finder_elems").children;

  // Add element.
  if (fuzzy_finder_inp.value == "" && event.keyCode == key_codes["+"])
    SelectAddElem(event);
  // Set modus to delete
  else if (objects.length != 0 && fuzzy_finder_inp.value == "" && event.keyCode == key_codes["-"])
    SelectDeleteModus(event);
  //ESC -> hide fuzzy finder
  else if (event.keyCode == key_codes["esc"]) {
    SelectCloseFinder();
    SelectCloseModals(); // Regarding this "fuzzy_finder" should be renamed to "key-driven" or something.
  }
  // Check if input is ment for an input field which isn't fuzzy finder.
  else if (NonFuzzyFinderInput(event))
    return;
  //Return -> Go to match
  else if (event.keyCode == key_codes["enter"] && pages.length > 0)
    SelectElement(event);
  // Select matches down.
  else if (event.keyCode == key_codes["down"])
    finder_counter = mod(finder_counter+1, pages.length); 
  // Select matches up.
  else if (event.keyCode == key_codes["up"])
    finder_counter = mod(finder_counter-1, pages.length); 
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
  delete_modus = true;
  document.getElementById("fuzzy_finder_mode").innerHTML = "delete: ";
  document.getElementById("fuzzy_finder_mode").style.color = "orange";
}

function SelectElement(event) {
  if (delete_modus == false)
    window.location = pages[finder_counter].innerHTML;
  else {
    event.preventDefault();
    OpenDelElemModal('subcategory', pages[finder_counter].innerHTML);
    SelectCloseFinder();
  }
}

function SelectCloseFinder() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  document.getElementById("fuzzy_finder_mode").innerHTML = "files: ";
  document.getElementById("fuzzy_finder_mode").style.color = "#1fe921";
  delete_modus = false;
  finder_counter = 0;
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
  checkMatch();
  if (event.target != fuzzy_finder_inp)
    document.getElementById("fuzzy_finder_div").style.display="block";
  fuzzy_finder_inp.focus();
}

function NonFuzzyFinderInput(event) {
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
}

function mod(n, m) {
  return ((n % m) + m) % m;
}
