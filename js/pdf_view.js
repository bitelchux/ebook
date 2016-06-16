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

var px = 0, vx = 0, ax = 0;
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

	if(vx - prev_vx > 15) {
		openPrevPage();
	}
  
	if(vx - prev_vx < -15) { 
		openNextPage();
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

var isPdfOn = false;



pageElement.addEventListener("pagebeforeshow", function() {
	isPdfOn = true;
	pageElement.style.display = 'block';
});



var lastTouchTime = 0;
pageElement.addEventListener('touchstart', function(e) {
	var touchPos = e.changedTouches[0];
	var touchTime = e.timeStamp;
	touchDown = true;

	if (touchTime - lastTouchTime < 500 &&
			Math.abs(touchPos.clientX - lastTouchPos.clientX) <= 20 &&
			Math.abs(touchPos.clientY - lastTouchPos.clientY) <= 20) {
		lastTouchTime = 0;
		toggleZoom();
	} else {
		lastTouchTime = touchTime;
	}
	lastTouchPos = touchPos;
});

pageElement.addEventListener('touchmove', function(e) {
	if (pageElement.scrollLeft === 0 ||
			pageElement.scrollLeft === pageElement.scrollWidth - page.clientWidth) {
				reachedEdge = true;
			} else {
				reachedEdge = false;
				touchStart = null;
			}

	if (reachedEdge && touchDown && !zoomed) {
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

window.addEventListener("rotarydetent", function(e) {
	if (!zoomed) {
		if (e.detail.direction == "CW")
			openNextPage();
		else
			openPrevPage();
	}
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
		if (!zoomed) {
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