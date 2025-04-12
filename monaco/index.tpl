<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>Monaco Editor</title>
<style>

html, body, #container {
  position: absolute;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  margin: 0;
  padding: 0;
  overflow: hidden;
}
</style>
<script src="https://public.codepenassets.com/js/prefixfree-1.0.7.min.js"></script>

</head>
<body>
<!-- partial:index.partial.html -->
<div id="container"></div>
<!-- partial -->
  <script src='https://unpkg.com/monaco-editor@0.8.3/min/vs/loader.js'></script>

<script>

require.config({ paths: { 'vs': 'https://unpkg.com/monaco-editor@0.8.3/min/vs' }});
window.MonacoEnvironment = { getWorkerUrl: () => proxy };

let proxy = URL.createObjectURL(new Blob([`
	self.MonacoEnvironment = {
		baseUrl: 'https://unpkg.com/monaco-editor@0.8.3/min/'
	};
	importScripts('https://unpkg.com/monaco-editor@0.8.3/min/vs/base/worker/workerMain.js');
`], { type: 'text/javascript' }));

require(["vs/editor/editor.main"], function () {
	let editor = monaco.editor.create(document.getElementById('container'), {
		value: [
                        {% for item in items %}
                        '{{ item }}',
                        {% endfor %}
		].join('\n'),
		language: '{{language}}',
		theme: 'vs-light' 
	});
	
	editor.addListener('didType', () => {
		console.log(editor.getValue());
	});
  
});
</script>

</body>
</html>
