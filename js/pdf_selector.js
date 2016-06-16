var hasLoaded = false;
var isSelectorOn = false;
var totalNumber = 0;
function clearSelector(){
	var selectorElement = document.getElementById("selector"),
		page = document.getElementById("selectorPage");
	child = selectorElement.childNodes;
	length = child.length;
	alert("Child Number" + child.length);
	var i = 0;
	for(i = length - 1;i >= 0; i--){
		alert("remove" + i);
		selectorElement.removeChild(child[i]);
	}

}


function pdfSelector(pdf,fullPath) {

	var page = document.getElementById("selectorPage"),
		selector = document.getElementById("selector"),
		selectorComponent,
		clickBound,
		indicator = page.querySelector(".custom-indicator"),
		mainText = indicator.querySelector(".main-text"),
		subText = indicator.querySelector(".sub-text"),
		PDF = pdf;
		filename = fullPath.substring(fullPath.lastIndexOf('/')+1);
		
		selector.setAttribute("data-item-degree", 35);
		selector.setAttribute("data-max-item-number", 10);
	
		function selectorChange(event) {
			var number = event.detail.title,
				itemId = event.detail.id;
			mainText.textContent = "Page";
		}
		
		
		
	function onClick(event) {
		isSelectorOn = false; 
		var target = event.target,
			number = target.getAttribute("data-title");
			if(number > 0 && number <= totalNumber){
				selector.removeEventListener("selectoritemchange",selectorChange,false);
				openPage(PDF,Number(number));
				tau.changePage("pdfPage",{
					allowSamePageTransition: true,
					transition: "flow"	
				});
			}

	}

	

	page.addEventListener("pagebeforeshow", function() {
		selector.addEventListener("selectoritemchange",selectorChange,false);
		isSelectorOn = true;
		if(!hasLoaded){
			onpage();
			hasLoaded = true;
			clickBound = onClick.bind(null);
			selectorComponent = tau.widget.Selector(selector);
			selector.addEventListener('click', clickBound, false);
		}
		else {}
			
	});
	page.addEventListener("pagebeforehide", function() {
		//selector.removeEventListener("click", clickBound, false);
		//window.removeEventListener("onpageshow", onpage, false);
	});
	
	function onpage(){
		subText.textContent = filename;
		totalNumber = pdf.numPages;
		var	selector = document.getElementById("selector");
		var addSelectorItem = function(component, number){
			var x = document.createElement("DIV");
			x.setAttribute('class','ui-item');
			x.setAttribute('data-title',number);
			x.id = number;
			component.appendChild(x);
		};
		var i = 0;
		var layer = 0;
		var max = PDF.numPages; 
		for(i = 0; i < max; i+=1){
			if(Math.floor(max / 10) == Math.floor(i / 10)) {
				addSelectorItem(selector,Math.floor(i/10)*10 + (max - i));
			} else {
				addSelectorItem(selector,Math.floor(i/10)*10 + (10 - i % 10));
			}
		}
	};
};