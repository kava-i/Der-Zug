//Close modal, when close-span is clicked (works for both moduls)
function CloseModul() {
  document.getElementById("modal_add_elem").style.display = "none";
  document.getElementById("modal_del_elem").style.display = "none";
  window.location=window.location;
}

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal1 = document.getElementById("modal_add_elem");
  let modal2 = document.getElementById("modal_del_elem");
  if (event.target == modal1) 
    modal1.style.display = "none";
  else if (event.target == modal2)
    modal2.style.display = "none";
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
  r_input_pw2.addEventListener("keyup", function(event) {
    if (event.keyCode === 13) {
      event.preventDefault();
      document.getElementById("btn_add_elem").click();
    }
  });
}

//Open modal to addd new controller, when button is clicked
function OpenAddElemModal() {
  document.getElementById("modal_add_elem").style.display = "block";
  document.getElementById("name").focus();
}

/**
 * Open modal to delete controller. 
 * Modal asks user, whether he/she really wants to delete controller.
 */
function OpenDelElemModal(type, name) {
  document.getElementById("modal_del_elem").style.display = "block";
  document.getElementById("check_msg").innerHTML = "Are you 100% sure "
    + "you want to delete <i>" + name + "</i>?";
  document.getElementById("check_msg").elem_name=name;
  document.getElementById("check_msg").elem_type=type;
}

/**
 * function to delete an element
 */
function AddElem(elem, force=false) {
  var json_request = new Object();
  json_request.name = document.getElementById("name").value;
  json_request.force = force;
  json_request.path = window.location.pathname;

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
    if (xhttp.status == 401 && this.responseText == "2") 
      msg.innerHTML = "You have no access to this file.";
    else if (xhttp.status == 401 && this.responseText  == "9") {
      msg.innerHTML = "Game is not running after this change";
      document.getElementById("force_add_elem").style.display = "inline-block";
    }
    else if (xhttp.status == 401) 
      msg.innerHTML = "Element couldn't be found or something went wrong.";
    //Display success message to user.
    else {
      msg.style= "display: block; color: green;"; 
      msg.innerHTML = "Successfully add " + json_request["name"];
      window.location = window.location;
    }
  }
}

/**
 * function deleting.
 */
function DelElem(what, name) {
  
  //Get controller name and path (url) from document.
  var json_request = new Object();
  json_request["name"] = document.getElementById("check_msg").elem_name;
  json_request["path"] = window.location.pathname;

  //Send request
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/delete_" + document.getElementById("check_msg").elem_type);
  xhttp.send(JSON.stringify(json_request));

  //Function to handle request 
  xhttp.onload = function(event){
    //If request fails, display message to user.
    if (xhttp.status == 401) {
      alert("Element couldn't be found or something went wrong.");
    }
    //Display success message to user.
    else {
      window.location=window.location;
    }
  }
}

function play_game(port) {
  var cur_loc = window.location.href;
  cur_loc = cur_loc.substr(0, cur_loc.indexOf(":", 7));
  window.open(cur_loc + ":" + parseInt(port) + "/");
}

function request_access(user, world) {
  var json_request = new Object();
  json_request["user"] = user;
  json_request["world"] = world;
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
