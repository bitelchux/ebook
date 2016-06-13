var ePage = document.getElementById("selectorPage");  
ePage.addEventListener('tizenhwkey', function(e) {
	  if (e.keyName === 'back') {
		 // window.removeEventListener('devicemotion', deviceMotionCallbackHandler(e), false);
	    try {
	    	selector.destroy();
	    	pdfPage = document.getElementById("pdfPage");
	    	pdfPage.style.display = "block";
	    	tau.changePage("main");
	    	//document.getElementById("selectorPage").style.display = 'none';
	    	//tau.changePage("main");
	    	//window.close();
	    	//tau.back();
	    	//$.mobile.changePage("#main",{
	    	//	allowSamePageTransition: true,
	    	//	transition: "fade"	
		    //});
		   } catch (error) {}
		 }
});

/*
(function() {
	var page = document.getElementById("selectorPage"),
		selector = document.getElementById("selector"),
		clickBound;

	function onClick(event) {
		var activeItem = selector.querySelector(".ui-item-active");
		//console.log(activeItem.getAttribute("data-title"));
	}
	page.addEventListener("pagebeforeshow", function() {
		clickBound = onClick.bind(null);
		tau.widget.Selector(selector);
		selector.addEventListener("click", clickBound, false);
	});
	page.addEventListener("pagebeforehide", function() {
		selector.removeEventListener("click", clickBound, false);
	});
})();
*/


function pdfSelector(pdf) {
	var page = document.getElementById("selectorPage"),
		selector = document.getElementById("selector"),
		selectorComponent,
		clickBound,
		indicator = page.querySelector(".custom-indicator"),
		mainText = indicator.querySelector(".main-text"),
		subText = indicator.querySelector(".sub-text"),
		PDF = pdf;

	function onClick(event) {
		var target = event.target,
			number = target.getAttribute("data-title");
		
		//console.log(activeItem.getAttribute("data-title"));
		
		openPage(PDF,Number(number));
		tau.changePage("pdfPage",{
	    		allowSamePageTransition: true,
	    		transition: "flow"	
 		 });
	
	
		/*
		 * Default indicator class selector is "ui-selector-indicator".
		 * If you want to show custom indicator sample code,
		 * check the 'customIndicator.js' please.
		 */
		if (target.classList.contains("ui-selector-indicator")) {
			alrert("onClick indicator");
			console.log("Indicator clicked");
			return;
		}
	}

	selector.addEventListener("selectoritemchange", function(event) {
		var layerIndex = event.detail.layerIndex,
			title = event.detail.title;
		mainText.textContent = title;
		subText.textContent = "Layer index " + layerIndex;
	});

	page.addEventListener("pagebeforeshow", function() {
		document.getElementById("selectorPage").style.display = 'block';
		onpage();
		clickBound = onClick.bind(null);
		selectorComponent = tau.widget.Selector(selector);
		selector.addEventListener('click', clickBound, false);

	});
	page.addEventListener("pagebeforehide", function() {
		selector.removeEventListener("click", clickBound, false);
		window.removeEventListener("onpageshow", onpage, false);
		selectorComponent.destroy();
	});
	
	function onpage(){
		var	selector = document.getElementById("selector");
		var addSelectorItem = function(component, number){
			var x = document.createElement("DIV");
			x.setAttribute('class','ui-item');
			x.setAttribute('data-title',number);
			//x.setAttribute('onclick', 'alert("Page Clicked")');
			x.innerHTML = number;
			component.appendChild(x);
		};
		var i = 0;
		var layer = 0;
		var max = PDF.numPages; 
		for(i = 0; i < max; i+=1){
			if(Math.floor(max / 11) == Math.floor(i / 11)) {
				addSelectorItem(selector,Math.floor(i/11)*11 + (max-i));
			}
			else {
				addSelectorItem(selector,Math.floor(i/11)*11 + (11 - i % 11));
			}
		}
		
	};


};
