//Close modal, when close-span is clicked (works for both moduls)
function CloseModul() {
  document.getElementById("modal_log").style.display = "none";

  //Show button again and empty log
  document.getElementById("get_log").style.display = "inline-block";
}

//Close modal when users clicks anywhere outside of the modal. (works for both)
window.onclick = function(event)  {
  let modal = document.getElementById("modal_log");
  if (event.target == modal) 
    modal.style.display = "none";
  else
    return;
  window.location=window.location;
}

//Function to send request, to check whether game is still running
function check_running() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/check_running");
  xhttp.send(window.location.pathname);
  
  xhttp.onload = function(event){
    document.getElementById("modal_log").style.display = "block";
    if (xhttp.status == 200)
      document.getElementById("check_msg").innerHTML = "Game is running as accpected!";
    else
      document.getElementById("check_msg").innerHTML = "Game is not running!";
  }
}

function get_log(x) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/get_"+x+"_log");
  console.log("sending request");
  xhttp.send(window.location.pathname);

  //Reset fields
  document.getElementById("display_log").innerHTML = "";
  document.getElementById("get_log").style.display = "block";

  xhttp.onload = function(event){
    console.log("Got response: ", this.responseText);
    if (xhttp.status == 200) {
      document.getElementById("log_div").style.display="block";
      document.getElementById("display_log").innerHTML = this.responseText;
      document.getElementById("get_log").style.display = "none";
    }
    else
      document.getElementById("check_msg").innerHTML = "failed getting log.";
  }
}

var ip = location.host.substr(0, location.host.indexOf(":"));
async function run(port) {
  //Check if game is already running.
  var adress = "ws://" + ip + ":" + parseInt(port+1) + "/";
  socket = new WebSocket(adress);
  await new Promise(r => setTimeout(r, 1000));
  console.log("Connection:", socket.readyState); 
  if (socket.readyState === 0 || socket.readyState === 1) {
    alert("Game already running.");
    return;
  }

  //Send request
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/start_game");
  xhttp.send(window.location.pathname);

  //Redirect to game
  xhttp.onload = function(event){
    //Fame already running 
    if (this.status != 200)
      alert("Unkown problem starting game.");

    //Start game if not already running.
    else {
      var cur_loc = window.location.href;
      cur_loc = cur_loc.substr(0, cur_loc.indexOf(":", 7));
      window.open(cur_loc + ":" + port + "/");
    }
  }
}

async function end(port) {
  //Check if game is already running.
  var adress = "ws://" + ip + ":" + parseInt(port+1) + "/";
  socket = new WebSocket(adress);
  await new Promise(r => setTimeout(r, 500));
  console.log("Connection:", socket.readyState); 
  if (socket.readyState === 2 || socket.readyState === 3) {
    document.getElementById("check_msg").innerHTML = "Game was not running.";
  }
  else {
    //End game.
    socket.send("l");
    socket.send("_admin");
    socket.send("password");
    socket.send("[end_game]");
    socket.send("[end_game]");
    await new Promise(r => setTimeout(r, 700));
    console.log("Connection:", socket.readyState); 
    if (socket.readyState === 2 || socket.readyState === 3)
      document.getElementById("check_msg").innerHTML = "Game closed succesfully!";
    else
      document.getElementById("check_msg").innerHTML = "Failed closing game!";
  }
  document.getElementById("modal_log").style.display = "block";
}
