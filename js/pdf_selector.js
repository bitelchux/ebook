var hasLoaded = false;
var isSelectorOn = false;
var totalNumber = 0;
function clearSelector(){
	var selectorElement = document.getElementById("selector"),
		page = document.getElementById("selectorPage");
	child = selectorElement.childNodes;
	var i = 0;
	for(i = 0;i < child.length; i+=1){
		selectorElement.removeChild(child[i]);
	}
		
	page.removeChild(selectorElement);
	newSelector = document.createElement("DIV");
	newSelector.setAttribute('class', 'ui-selector');
	newSelector.id = "selector";
	page.appendChild(newSelector);
}


function pdfSelector(pdf) {

	var page = document.getElementById("selectorPage"),
		selector = document.getElementById("selector"),
		selectorComponent,
		clickBound,
		indicator = page.querySelector(".custom-indicator"),
		mainText = indicator.querySelector(".main-text"),
		subText = indicator.querySelector(".sub-text"),
		PDF = pdf;

		selector.setAttribute("data-item-degree", 35);
		selector.setAttribute("data-max-item-number", 10);

	function onClick(event) {
		isSelectorOn = false; 
		var target = event.target,
			number = target.getAttribute("data-title");
			openPage(PDF,Number(number));
			tau.changePage("pdfPage",{
				allowSamePageTransition: true,
				transition: "flow"	
			});

	}

	selector.addEventListener("selectoritemchange", function(event) {
		var number = event.detail.title,
			itemId = event.detail.id;
		mainText.textContent = "Page " + number;
		subText.textContent = itemId;
	});

	page.addEventListener("pagebeforeshow", function() {
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
		totalNumber = pdf.numPages;
		var	selector = document.getElementById("selector");
		var addSelectorItem = function(component, number){
			var x = document.createElement("DIV");
			x.setAttribute('class','ui-item');
			x.setAttribute('data-title',number);
			x.id = number;
			x.innerHTML = number;
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