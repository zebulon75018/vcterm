<!DOCTYPE html>
<html lang="en" >
<head>
  <meta charset="UTF-8">
  <title>awesom gallery</title>

</head>
<body>
<!-- partial:index.partial.html -->
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <title>Image Gallery</title>
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/gh/fancyapps/fancybox@3.5.7/dist/jquery.fancybox.min.css" />
<style>

* {
  padding: 0;
  margin: 0;
  text-decoration: none;
  list-style-type: none;
  box-sizing: border-box;
}

body {
  background: #fff;
  font-family: Trebuchet Ms;
}

header {
  text-align: center;
  margin-bottom: 2rem;
  margin-top: 2rem;
}

.container {
  width: 80%;
  column-count: 4;
  margin: 0 auto;
}

.grid-item img {
  width: 100%;
  height: auto;
  margin: 1em;
}

@media only screen and (max-width: 768px) {
  .container {
    column-count: 3;
  }
}

@media only screen and (max-width: 450px) {
  .container {
    column-count: 2;
  }
}
</style>
</head>

<body>
  <div class="container">
   {% for f in files %}
    <div class="grid-item">
      <a data-fancybox="gallery" href="{{f}}"><img src="{{f}}"></a>
    </div>
   {% endfor %}
  </div>

  <script src="https://cdn.jsdelivr.net/npm/jquery@3.4.1/dist/jquery.min.js"></script>
  <script src="https://cdn.jsdelivr.net/gh/fancyapps/fancybox@3.5.7/dist/jquery.fancybox.min.js"></script>
</body>

</html>
<!-- partial -->
  
</body>
</html>
