if (!window.requestAnimationFrame) {
  window.requestAnimationFrame = (function() {
    return window.webkitRequestAnimationFrame ||
      window.mozRequestAnimationFrame ||
      window.oRequestAnimationFrame ||
      window.msRequestAnimationFrame ||
      function(callback, element) {
        window.setTimeout(callback, 1000 / 60);
      };
  })();
}


var px = 0, vx = 0,  ax = 0;
var prev_vx= 0;
var lastExecution=0;
window.addEventListener('devicemotion', function(e) {
	deviceMotionCallbackHandler(e);
	
}, true);

function deviceMotionCallbackHandler(e) {		
	 //Calculate the acceleration due to gravity
	  var now = Date.now();
	  if(lastExecution !==0 && now-lastExecution <1000) return;
  ax = e.accelerationIncludingGravity.x *5 ;
  vx = vx + ax;
  vx = vx * 0.98;
  var rate = e.rotationRate.beta;

  if(vx - prev_vx > 15 && vx >= 0) {
      openNextPage();
  }
  
  if(vx - prev_vx < -15 && vx <= 0) { 
	  openPrevPage();
  }
  
  prev_vx = vx;
  lastExecution = now;
};


var pdf;
var canvas = document.getElementById('canvas');
var context = canvas.getContext('2d');
var pageElement = document.getElementById('pdfPage');
var reachedEdge = false;
var touchStart = null;
var touchDown = false;

var lastTouchTime = 0;


document.addEventListener('tizenhwkey', function(e){
	if (e.keyName === 'back'){
		pageElement.style.display = "none";
		pdfSelector(pdf);
		tau.changePage("selectorPage");
		

	}
}
);
pageElement.addEventListener("pagebeforeshow", function() {
	pdfPage.style.display = "block";
});


pageElement.addEventListener("pagebeforehide", function() {
	//document.removeEventListener("tizenhwkey", pdfBackClicked, false);
	pageElement.removeChild(canvas);
	var newCanv = document.createElement('canvas');
	newCanv.id = 'canvas';
	pageElement.appendChild(newCanv);
	tau.changePage("selectorPage");
});

pageElement.addEventListener('touchstart', function(e) {
  touchDown = true;

  if (e.timeStamp - lastTouchTime < 500) {
    lastTouchTime = 0;
    toggleZoom();
  } else {
    lastTouchTime = e.timeStamp;
  }
});

pageElement.addEventListener('touchmove', function(e) {
  if (pageElement.scrollLeft === 0 ||
    pageElement.scrollLeft === pageElement.scrollWidth - pageElement.clientWidth) {
    reachedEdge = true;
  } else {
    reachedEdge = false;
    touchStart = null;
  }

  if (reachedEdge && touchDown) {
    if (touchStart === null) {
      touchStart = e.changedTouches[0].clientX;
    } else {
      var distance = e.changedTouches[0].clientX - touchStart;
      if (distance < -100) {
        touchStart = null;
        reachedEdge = false;
        touchDown = false;
        openNextPage();
      } else if (distance > 100) {
        touchStart = null;
        reachedEdge = false;
        touchDown = false;
        openPrevPage();
      }
    }
  }
});


pageElement.addEventListener('touchend', function(e) {
  touchStart = null;
  touchDown = false;
});

var pdfFile;
var currPageNumber = 1;

var openNextPage = function() {
  var pageNumber = Math.min(pdfFile.numPages, currPageNumber + 1);
  if (pageNumber !== currPageNumber) {
    currPageNumber = pageNumber;
    openPage(pdfFile, currPageNumber);
  }
};

var openPrevPage = function() {
  var pageNumber = Math.max(1, currPageNumber - 1);
  if (pageNumber !== currPageNumber) {
    currPageNumber = pageNumber;
    openPage(pdfFile, currPageNumber);
  }
};

var zoomed = false;
var toggleZoom = function () {
	  zoomed = !zoomed;
	  var viewport = $('meta[name="viewport"]');
	  if (zoomed) {
	      viewport.attr("content", "width=360, initial-scale=1"); 
	  } else {
	      viewport.attr("content", "width=360, initial-scale=10"); 
	  }
	  openPage(pdfFile, currPageNumber);
};

var fitScale = 1;
var renderTask;
var openPage = function(pdfFile, pageNumber) {
	pdf = pdfFile;
	var scale = zoomed ? fitScale : 1;
	var canvas = document.getElementById('canvas');
	var context = canvas.getContext('2d');
	pdfFile.getPage(pageNumber).then(function(page) {
		if(renderTask){
			renderTask.cancel();
		}
		viewport = page.getViewport(1);
		if (zoomed) {
		   		var scale = pageElement.clientWidth / viewport.width;
           		viewport = page.getViewport(scale);
    	}
		

		canvas.height = viewport.height;
		canvas.width = viewport.width;

		var renderContext = {
				canvasContext: context,
				viewport: viewport,
				continueCallback: function(cont){
					cont();
				}
		};
		renderTask = page.render(renderContext);
  });
};


