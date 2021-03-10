var paths = []
var objects = []

window.onload = function() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  paths = document.getElementById("fuzzy_finder_div").getAttribute("pages");
  paths = JSON.parse(paths);
  if (document.getElementById("fuzzy_finder_div").hasAttribute("page_objects")) {
    page_objects = [];
    page_objects = document.getElementById("fuzzy_finder_div").getAttribute("page_objects");
    page_objects = JSON.parse(page_objects)
    console.log(page_objects);
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
  var inps = []
  if (delete_modus == false) 
    inps = paths;
  else 
    inps = objects;
  for (var i=0; i<inps.length; i++) {
    var current_page = inps[i].toLowerCase();

    //If not found, continue
    if (current_page.indexOf(inp) == -1) continue;

    // Calculate score.
    var score = current_page.length/inp.length;
    if (delete_modus == false) {
      if (current_page.indexOf(window.location.pathname) != -1) score/=2;
      last_elem = current_page.substr(current_page.lastIndexOf("/"));
      if (last_elem.indexOf(inp) == 0) score/=2;
    }
    matches.push([inps[i], score])
  }

  // Sort and take only first 8 elements
  matches.sort(function(a,b) { return a[1] - b[1]; });
  matches = matches.slice(0,8);

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

var finder_counter = 0;
var delete_modus = false;
document.addEventListener('keydown', function(event) {

  fuzzy_finder_inp = document.getElementById("fuzzy_finder_inp");
  pages = document.getElementById("fuzzy_finder_elems").children;

  // Add element.
  if (objects.length != 0 && fuzzy_finder_inp.value == "" && event.keyCode == 61 ) {
    event.preventDefault();
    OpenAddElemModal();
    document.getElementById("name").value = "";
    return;
  }

  // Set modus to delete
  if (objects.length != 0 && fuzzy_finder_inp.value == "" && event.keyCode == 173) {
    console.log("Delete modus triggered: ", event.keyCode);
    delete_modus = true;
    document.getElementById("fuzzy_finder_mode").innerHTML = "delete: ";
    document.getElementById("fuzzy_finder_mode").style.color = "orange";
  }

  // Check if input is ment for an input field which isn't fuzzy finder.
  var input_fields = [];
  input_fields.push.apply(input_fields, document.getElementsByTagName('input'));
  input_fields.push.apply(input_fields, document.getElementsByTagName('textarea'));
  for (var index = 0; index < input_fields.length; ++index) {
    if (event.target == input_fields[index] && event.target != fuzzy_finder_inp) 
      return;
  }

  if (event.keyCode == 27) //ESC -> hide fuzzy finder
    CloseFinder();

  else if (event.keyCode == 13 && pages.length > 0) {//Return -> Go to match
    if (delete_modus == false)
      window.location = pages[finder_counter].innerHTML;
    else {
      event.preventDefault();
      OpenDelElemModal('subcategory', pages[finder_counter].innerHTML);
      CloseFinder();
    }
  }

  // Select match
  else if (event.keyCode == 40) {
    if (finder_counter < pages.length) finder_counter+=1; 
  }
  else if (event.keyCode == 38) {
    if (finder_counter > 0) finder_counter-=1; 
  }

  // Add letter to entry 
  else if ((event.keyCode >= 65 && event.keyCode <= 90) 
      || event.keyCode == 55 || event.keyCode == 222 || event.keyCode == 59
      || event.keyCode == 219) {
    checkMatch();
    if (event.target != fuzzy_finder_inp) {
      document.getElementById("fuzzy_finder_div").style.display="block";
    }
    fuzzy_finder_inp.focus();
  }

  if (fuzzy_finder_inp.value=="")
    document.getElementById("fuzzy_finder_elems").innerHTML = "";

  HighlightElem();
}, true);

function CloseFinder() {
  document.getElementById("fuzzy_finder_div").style.display="none";
  document.getElementById("fuzzy_finder_inp").value = "";
  document.getElementById("fuzzy_finder_elems").innerHTML = "";
  document.getElementById("fuzzy_finder_mode").innerHTML = "files: ";
  document.getElementById("fuzzy_finder_mode").style.color = "#1fe921";
  delete_modus = false;
  finder_counter = 0;
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
