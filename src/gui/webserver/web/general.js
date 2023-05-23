// error codes
var error_codes = {"0":"success", "1":"wrong format", "2":"access denied", 
  "3":"path/ object not found.", "4":"action not allowed", "5":"already exists",
  "6":"failed", "7":"no user", "8":"no world", "9":"game not running"};

//Close modal, when close-span is clicked (works for both moduls)
function CloseModul() {
  try {
    document.getElementById("modal_add_elem").style.display = "none";
    document.getElementById("modal_del_elem").style.display = "none";
  } catch(e) {}

  // Handle log-modal.
  let log_modal = document.getElementById("modal_log");
  if (log_modal != null && log_modal != undefined) {
    log_modal.style.display= "none";
    document.getElementById("get_log").style.display = "inline-block"; // show button again.
  }
  window.location=window.location;
}

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal1 = document.getElementById("modal_add_elem");
  let modal2 = document.getElementById("modal_del_elem");
  let modal3 = document.getElementById("modal_log");
  let modal4 = document.getElementById("modal_write");
  if (event.target == modal1) 
    modal1.style.display = "none";
  if (event.target == modal2)
    modal2.style.display = "none";
  if (event.target == modal3)
    modal3.style.display = "none";
  if (event.target == modal4)
    modal4.style.display = "none";
  else
    return;
  window.location=window.location;
}

/**
 * Trigger button click by pressing enter, when on last input field
 */
window.onload = function() {
  if (document.getElementById("modal_add_elem").style.display == "none")
    return;
  var r_input_pw2 = document.getElementById("name");
	if (document.getElementById("name").placeholder != "character id") {
		r_input_pw2.addEventListener("keyup", function(event) {
			if (event.keyCode === 13) {
					event.preventDefault();
					document.getElementById("btn_add_elem").click();
			}
		});
	}
}

//Open modal to addd new controller, when button is clicked
function OpenAddElemModal() {
  document.getElementById("modal_add_elem").style.display = "block";
  document.getElementById("name").focus();

  //Check if player, then add field to enter name of start-room.
  if (window.location.pathname.indexOf("players/players") != -1)
    document.getElementById("room").style.display = "inline-block";
  if (window.location.pathname.indexOf("overview") != -1)
    document.getElementById("language").style.display = "inline-block";

  document.getElementById("name").addEventListener("keyup", function(event) {
    if (event.keyCode === 13) {
      event.preventDefault();
      document.getElementById("btn_add_elem").click();
    }
  });
}

/**
 * Open modal to delete controller. 
 * Modal asks user, whether he/she really wants to delete controller.
 */
function OpenDelElemModal(type, name) {
	try {
		document.getElementById("modal_del_elem").style.display = "block";
		document.getElementById("check_msg").innerHTML = "Are you 100% sure "
			+ "you want to delete <i>" + name + "</i>?";
		document.getElementById("check_msg").elem_name=name;
		document.getElementById("check_msg").elem_type=type;
		document.getElementById("btn_del_elem").focus();
	} catch(e) {
		console.log("OpenDelElemModal failed: ", e);
	}
}

/**
 * function to add an element
 */
function AddElem(elem, force=false) {
  var json_request = new Object();
  json_request.name = document.getElementById("name").value;
  json_request.force = force;
  json_request.path = window.location.pathname;
  json_request.infos = {};
  // Add values from all inputs
  var inputs = document.getElementById("modal_add_elem").getElementsByTagName("input");
  for (let i=0; i<inputs.length; i++) {
    const val = inputs[i].value.replaceAll("/", "_").replaceAll(" ", "-");
    json_request["infos"][inputs[i].id] = val;
  }
  // Add values from all selects
  var selects = document.getElementById("modal_add_elem").getElementsByTagName("select");
  for (let i=0; i<selects.length; i++) {
    const val = selects[i].value.replaceAll("/", "_").replaceAll(" ", "-");
    json_request["infos"][selects[i].id] = val;
  }

  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/add_"+elem);
  xhttp.send(JSON.stringify(json_request));
  
  //Function to handle request 
  xhttp.onload = function(event){
    
    //If request fails, display message to user.
    let msg = document.getElementById("add_error");
    console.log(event.data);
    if (xhttp.status != 200) {
      msg.style = "display: block;"; 
      document.getElementById("btn_add_elem").style.display = "none";
    }
    if (xhttp.status == 401 && this.responseText in error_codes) {
      msg.innerHTML = "Adding element failed due to error: " + error_codes[this.responseText];
      if (this.responseText == "9") {
        document.getElementById("force_add_elem").style.display = "inline-block";
        document.getElementById("add_get_log").style.display = "inline-block";
      }
    }
    else if (xhttp.status == 401) 
      msg.innerHTML = "Element couldn't be found or something went wrong.";
    //Display success message to user.
    else {
      msg.style= "display: block; color: green;"; 
      msg.innerHTML = "Successfully add " + json_request["name"];
      sessionStorage.setItem("notification", "Element added.");
      window.location = window.location;
    }
  }
}

/**
 * function to add an element
 */
function AddMedia(media_type, force=false) {
  let formData = new FormData();
  let req_data = {'name': document.getElementById("name").value, "path": window.location.pathname };
  let media = document.getElementById("media_file").files[0];  
  console.log("request data: ", req_data);
  console.log("media: ", media);
  formData.append("data", JSON.stringify(req_data));
  formData.append("photo", media); 

  console.log(...formData);

  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/upload/"+media_type);
  xhttp.send(formData);
  xhttp.onload = function(event) {
    if (xhttp.status == 200)
      notify("Media file upload.");
    else
      notify("Media file could not be uploaded.");
  }
}

/**
 * function deleting.
 */
function DelElem(force=false) {
  //Get controller name and path (url) from document.
  var json_request = new Object();
  json_request["name"] = document.getElementById("check_msg").elem_name;
  json_request["path"] = window.location.pathname;
  json_request["force"] = force;

  //Send request
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/delete_" + document.getElementById("check_msg").elem_type);
  xhttp.send(JSON.stringify(json_request));

  //Function to handle request 
  xhttp.onload = function(event){
    let msg = document.getElementById("del_error");
    //If request fails, display message to user.
    if (xhttp.status != 200) {
      msg.style = "display: block;"; 
      document.getElementById("btn_del_elem").style.display = "none";
      // got error code.
      if (this.responseText in error_codes) {
        msg.innerHTML = "Deleteing element failed due to error: " + error_codes[this.responseText];
        if (this.responseText == "9") {
          document.getElementById("force_del_elem").style.display = "inline-block";
          document.getElementById("del_get_log").style.display = "inline-block";
        }
      }
      // no error code given.
      else 
        msg.innerHTML = "Element couldn't be found or something went wrong.";
    }
    //Display success message to user.
    else {
      sessionStorage.setItem("notification", "Element removed.");
      window.location=window.location;
    }
  }
}

function play_game(creator, world_name, textad_port) {
  // Check if game is running.
  const data = new Object({"creator": creator, "world_name": world_name});
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/running");
  xhttp.send(JSON.stringify(data));
  xhttp.onload = function(event){
    // Game already running 
    if (this.status != 200) {
      notify("Game not running.");
      return;
    }
    else {
      // Redirect to game
      const hostname = window.location.hostname;
      window.open("http://" + hostname + ":" + textad_port + "/" + creator + "/" + world_name);
    }
  }
}

function request_access(user, world) {
  var json_request = new Object();
  json_request["user"] = user;
  json_request["world"] = world;
  k
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/create_request");
  xhttp.send(JSON.stringify(json_request));

  xhttp.onload = function(event){
    //If request fails, display message to user.
    if (xhttp.status == 401) {
      alert("Sending request failed");
    }
    //Display success message to user.
    else {
      alert("Successfully sent request");
    }
  }
}

function GrantAccessTo(user, world) {
  var json_request = new Object();
  json_request["user"] = user;
  json_request["world"] = world;

  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/grant_access_to");
  xhttp.send(JSON.stringify(json_request));

  xhttp.onload = function(event){
    //If request fails, display message to user.
    if (xhttp.status == 401) {
      alert("granting accesss failed");
    }
    //Display success message to user.
    else {
      alert("Successfully granted access");
    }
  }
}

function ToggleGraph() {
  SetNotes();
  var tab_elemens = document.getElementById("tab_elemens");
  var tab_graph = document.getElementById("tab_graph");
  var tab_notes = document.getElementById("tab_notes");
  
  if (tab_elemens.style.display === "block" || tab_notes.style.display === "block") {
    console.log("switched to graph view");
    tab_elemens.style.display = "none";
    tab_graph.style.display = "block";

    // Get graph from object.
    var xhttp = new XMLHttpRequest();
    var query = "?type=graph&path=" + window.location.pathname;
    console.log("Query: " + query);
    xhttp.open("GET", "/api/get_object" + query);
    xhttp.send();
    
    //Function to handle request 
    xhttp.onload = function(event){
      //If request fails, display message to user.
      console.log(this.responseText);
      if (this.responseText == "{}") {
        alert("No graph availibe.");
        ToggleGraph();
      }
      else if (xhttp.status == 200)
        LoadGraph(JSON.parse(this.responseText));
      else {
        alert("could not load notes.");
        ToggleGraph();
      }
    }
  }
  else {
    console.log("switched to list view");
    tab_elemens.style.display = "block";
    tab_graph.style.display = "none";
  }
  tab_notes.style.display = "none";
}

function ToggleNotes() {
  var tab_elemens = document.getElementById("tab_elemens");
  var tab_notes = document.getElementById("tab_notes");
  var tab_graph = document.getElementById("tab_graph");

  if (tab_elemens.style.display === "block" || tab_graph.style.display === "block") {
    console.log("switched to graph view");
    tab_elemens.style.display = "none";
    tab_notes.style.display = "block";

    // Get graph from object.
    var xhttp = new XMLHttpRequest();
    var query = "?type=notes&path=" + window.location.pathname;
    console.log("Query: " + query);
    xhttp.open("GET", "/api/get_object" + query);
    xhttp.send();
    
    //Function to handle request 
    xhttp.onload = function(event){
      //If request fails, display message to user.
      console.log(this.responseText);
      if (xhttp.status == 200) {
        var notes = this.responseText;
        if (notes.length >= 2)
          notes = notes.substr(1, notes.length-2);
        document.getElementById("notes").innerHTML = notes;
        let md_notes = notes.replaceAll("<div>", "\n");
        md_notes = md_notes.replaceAll("</div>", "");
        md_notes = md_notes.replaceAll("<br>", "\n");
        console.log(md_notes);
        console.log(marked(md_notes));
        document.getElementById("notes_md").innerHTML = marked(md_notes);
      }
      else {
        alert("could not load .");
        ToggleNotes();
      }
    }
  }
  else {
    console.log("switched to list view");
    tab_elemens.style.display = "block";
    tab_notes.style.display = "none";
    SetNotes();
  }
  tab_graph.style.display = "none";
}

function ToggleMarkdown() {
  SetNotes();
  console.log("ToggleMarkdown");
  if (document.getElementById("tab_notes").style.display === "none") {
    return;
  }

  var notes = document.getElementById("notes");
  var md_notes = document.getElementById("notes_md");

  if (notes.style.display === "none") {
    notes.style.display = "block";
    md_notes.style.display = "none";
  }

  else if (md_notes.style.display === "none") {
    md_notes.style.display = "block";
    notes.style.display = "none";
  }
}

function SetNotes() {
  // Get graph from object.
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/set_notes");
  var notes = document.getElementById("notes").innerHTML;
  if (notes.length < 1)
    return;
  var request = {"path": window.location.pathname, "notes":notes};
  console.log(request);
  xhttp.send(JSON.stringify(request));
  xhttp.onload = function() {
    if (xhttp.status === 200)
      notify("Notes updated.");
    else
      notify("Failed to update notes.");
  }
}

notification_running_ = false;
async function notify(text, color) {
  while(notification_running_ === true)
    await new Promise(r => setTimeout(r, 200));
  notification_running_ = true;
    
  let notification = document.getElementById("notification");
  notification.innerHTML = text;
  unfade(notification);
  await new Promise(r => setTimeout(r, 1500));
  fade(notification);
  notification_running_ = false;
}

function unfade(element) {
    var op = 0.1;  // initial opacity
    element.style.display = 'block';
    var timer = setInterval(function () {
        if (op >= 1){
            clearInterval(timer);
        }
        element.style.opacity = op;
        element.style.filter = 'alpha(opacity=' + op * 100 + ")";
        op += op * 0.1;
    }, 20);
}

function fade(element) {
    var op = 1;  // initial opacity
    var timer = setInterval(function () {
        if (op <= 0.1){
            clearInterval(timer);
            element.style.display = 'none';
        }
        element.style.opacity = op;
        element.style.filter = 'alpha(opacity=' + op * 100 + ")";
        op -= op * 0.1;
    }, 50);
}
