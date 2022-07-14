# Webserv

error_page 404 html/error/404.html;

```
server {
    host 127.0.0.1;
	port 8080;
    server_name testserver;
	
	location / {
		root html;
		allowed_method GET;
		index index.html;
		autoindex off;
		cgi_path ;
	}

	error_page 404 html/error/404.html;

}
```
