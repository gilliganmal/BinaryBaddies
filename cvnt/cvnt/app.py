from build_flask_app import build_app

app = build_app()
app.run('0.0.0.0' , debug = True)
