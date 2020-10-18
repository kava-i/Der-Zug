//Sends a request to create a backup to the server
function create_backup() {
  var path = window.location.pathname;
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/create_backup");
  xhttp.send(path.substr(7));
  xhttp.onload = function(event){    
    var error_msg = document.getElementById("create_backup_error");
    if (xhttp.status != 200) {
      error_msg.innerHTML = "Adding backup failed. Please try again later or contect an admin.";
      error_msg.style = "display: block; color:red;";
    }
    else {
      error_msg.innerHTML = "Successfully added backup.";
      error_msg.style = "display: block; color:green;";
    }
  }
}

//Opens a modal to show all existing backups
function show_backups() {
  var path = window.location.pathname;
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/api/show_back");
  xhttp.send(path.substr(7));
  xhttp.onload = function(event){    
    var error_msg = document.getElementById("create_backup_error");
    if (xhttp.status != 200) {
      error_msg.innerHTML = "Adding backup failed. Please try again later or contect an admin.";
      error_msg.style = "display: block; color:red;";
    }
    else {
      error_msg.innerHTML = "Successfully added backup.";
      error_msg.style = "display: block; color:green;";
    }
  }
}
