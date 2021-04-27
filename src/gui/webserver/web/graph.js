function LoadGraph(data) {
  document.getElementById("cy").innerHTML = "";

  // Set the dimensions and margins of the graph
  var margin = {top: 10, right: 10, bottom: 10, left:10},
    width = 400 - margin.left - margin.right,
    height = 600 - margin.top - margin.bottom;

  // append the svg object to the body of the page
  var svg = d3.select("#cy")
    .append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top - margin.bottom)
    .append("g")
      .attr("transform",
        "translate(" + margin.left + "," + margin.top + ")");

  var color = function(d) {
    var colors = ["#283747", "#5d6d7e", "#5f6a6a", "#aab7b8", "#641e16"]
    return colors[d.group];
  }  

  var drag = function(simulation) { 
    return simulation => {
      function dragstarted(event) {
        if (!event.active) simulation.alphaTarget(0.3).restart();
        event.subject.fx = event.subject.x;
        event.subject.fy = event.subject.y;
      }
      
      function dragged(event) {
        event.subject.fx = event.x;
        event.subject.fy = event.y;
      }
      
      function dragended(event) {
        if (!event.active) simulation.alphaTarget(0);
        event.subject.fx = null;
        event.subject.fy = null;
      }
      
      return d3.drag()
          .on("start", dragstarted)
          .on("drag", dragged)
          .on("end", dragended);
    }
  }

  // d3.json("https://raw.githubusercontent.com/holtzy/D3-graph-gallery/master/DATA/data_network.json", function(data) {
  // Initialize links
  var link = svg
      .attr("stroke", "#999")
      .attr("stroke-opacity", 0.6)
    .selectAll("line")
    .data(data.links)
    .enter()
    .append("line")
      .style("stroke", "#aaa")

  // Initialize nodes
  var node = svg 
      .attr("stroke", "#fff")
      .attr("stroke-width", 1.5)
    .selectAll("circle")
    .data(data.nodes)
    .enter()
    .append("circle")
      .attr("r", 10)
      .attr("fill", color)
      .call(drag(simulation))

  node.append("title")
    .text(d => d.id);

  // Let's list the force we wanna apply on the network
  var simulation = d3.forceSimulation(data.nodes)           // Force algorithm is applied to data.nodes
    .force("link", d3.forceLink()                           // This force provides links between nodes
      .id(function(d) {return d.id; })                      // This provides the id of a node
      .links(data.links)                                    // And this the list of links
    )
    .force("charge", d3.forceManyBody().strength(-100))     // Adds repulstion between nodes. 
    .force("center", d3.forceCenter(width / 2, height / 2)); // This force attracts nodes to the center.

  // This function is run at each interation of the force algorithm, updating the nodes position.
  simulation.on("tick", () => {
    link
      .attr("x1", function(d) {return d.source.x; })
      .attr("y1", function(d) {return d.source.y; })
      .attr("x2", function(d) {return d.target.x; })
      .attr("y2", function(d) {return d.target.y; })

    node
      .attr("cx", function(d) {return d.x; })
      .attr("cy", function(d) {return d.y; })
  });
}
