// import 'dart:math';
import 'dart:async';

import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter_speed_dial/flutter_speed_dial.dart';
// import 'package:flutter_circular_slider/flutter_circular_slider.dart';
import 'package:sleek_circular_slider/sleek_circular_slider.dart';
import 'dart:developer' as developer;

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  final Future<FirebaseApp> _fbApp = Firebase.initializeApp();
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: FutureBuilder(
        future: _fbApp,
        builder: (context,snapshot)
        {
          if(snapshot.hasError)
            {
              print('You have an error ${snapshot.error.toString()}');
              return Text('Something went wrong!');
            }
          else if(snapshot.hasData)
            {
              return MyHomePage();
            }
          else
            {
              return Center(
                child : CircularProgressIndicator(),
              );
            }
        },
      )
    );
  }
}

class MyHomePage extends StatefulWidget {
  // MyHomePage({Key key, this.title}) : super(key: key);

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  // final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {

  Timer  _everySecond;
  double value1 = 0;
  String txt='',temp='0°C',hum="16%";
  final databaseRef = FirebaseDatabase.instance.reference();
  String sgspd = '';
  bool _toggled =false;
  bool _toggled1 = false;
  String err='',dis='Welcome';
  bool _isvisible =true;
  String abc ='';
  @override
  void initState() {
    // TODO: implement initState
    super.initState();
      databaseRef.child('android').child('test').once().then((DataSnapshot snapshot)
      {
        double k = double.parse(snapshot.value);
        value1 = k;
        developer.log("Logmeya",error: value1);
        if(value1>0)
        {
          _toggled =true;
        }
        refresh1();
      });
      databaseRef.child('android').child('mode').once().then((DataSnapshot snapshot)
      {
        if(snapshot.value == 'AUTO')
          {
            _toggled1 =true;
          }
        else
          {
            _toggled1 =false;
          }
        refresh1();
      });
    _everySecond = Timer.periodic(Duration(seconds: 1), (Timer t) {
      setState(() {
        databaseRef.child('controller').child('temp').once().then((DataSnapshot snapshot)
        {
          temp = snapshot.value;
          print('temp:${temp}');
        });
        databaseRef.child('controller').child('hum').once().then((DataSnapshot snapshot)
        {
          hum = snapshot.value;
          print('hum:${hum}');
        });
        databaseRef.child('controller').child('command').once().then((DataSnapshot snapshot) {
          sgspd  = snapshot.value;
          print('${sgspd}');
          // refresh();
        });
        databaseRef.child('controller').child('test').once().then((DataSnapshot snapshot) {
          abc = snapshot.value;
          refresh();
        });
      });
    });
    // value1 = 3;
    // _toggled =true;
  }
  // print('temp:${temp}');
 void refresh()
 {
   // Future.delayed(Duration(seconds: 1)).then((_) {
     setState(()
     {
       // databaseRef.child('controller').child('temp').once().then((DataSnapshot snapshot)
       // {
       //   temp = snapshot.value;
       //   print('temp:${temp}');
       // });
       // databaseRef.child('controller').child('hum').once().then((DataSnapshot snapshot)
       // {
       //   hum = snapshot.value;
       //   print('hum:${hum}');
       // });
       // databaseRef.child('controller').child('command').once().then((DataSnapshot snapshot) {
       //   sgspd  = snapshot.value;
       //   print('${sgspd}');
       //   refresh1();
       // });
     });
   //   refresh();
   // });

 }
 void refresh1()
 {
   setState(() {
     databaseRef.reference().child('android').child('error').once().then((DataSnapshot snapshot)
     {
       err= snapshot.value;
     });
   });

 }
void auto(bool val)
{

  // auto(val);
}
 void refreshAuto(){
   setState(()
   {
      databaseRef.reference().child('controller').child('test').once().then((DataSnapshot snapshot) {
      double k=double.parse(snapshot.value);
      value1 = k;
      });
   });
}
  @override
  Widget build(BuildContext context) {
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        // Here we take the value from the MyHomePage object that was created by
        // the App.build method, and use it to set our appbar title.
        leading: Icon(Icons.android),
        title: Text("Smart Fan"),
        backgroundColor: Colors.green,
      ),
      body: new Stack(
        children: [
          Visibility(
          visible: (!_toggled1),
          child: new Align(
          child:new Container(
            alignment :Alignment(0.0,-0.20),
            child : new SleekCircularSlider(
              appearance: CircularSliderAppearance(
                  customWidths: CustomSliderWidths(progressBarWidth: 10),
                  infoProperties: InfoProperties(modifier: ((double value) {
              final roundedValue = value1.ceil().toInt().toString();
              return '$roundedValue';
              }))
              ),
              min: 0,
              max: 5,
              initialValue: value1,
              onChange: ((double val)
              {
                setState(() {
                  if(val<=0.5)
                  {
                    value1 =0;
                  }
                  else if (val<=1.5)
                  {
                    value1 = 1;
                  }
                  else if (val<=2.5)
                  {
                    value1 =2;
                  }
                  else if(val<=3.5)
                  {
                    value1 =3;
                  }
                  else if (val<=4.5)
                  {
                    value1 =4;
                  }
                  else
                  {
                    value1 =5;
                  }
                  databaseRef.reference().child('android').child('test').set('${value1}');
                  if(value1>0)
                    {
                      _toggled = true;
                    }
                  else
                    {
                      _toggled =false;
                    }
                  databaseRef.reference().child('android').child('switch').set('${_toggled ? 'ON':'OFF'}');
                });
              }),
            )
            )
          ),
            replacement: Align(
              alignment: Alignment(0.0,-0.3),
              child: new Container(
                child: new Text('Fan Speed is ${abc}'),
              ),
            ),
          ),
          // Align(
          //   alignment:Alignment(0.0,0.1),
          //   child: new Text('Fan Speed is ${value1.round().toString()}'),
          // ),
          Align(
            alignment: Alignment(0.0,0.25),
            child: new SwitchListTile(
              title: new Text("OFF/ON"),
                value: _toggled,
                onChanged: (bool val)
             {
              setState(()
             {
                _toggled =val;
                if(val)
                {
                  value1 = 1;
                }
                else
                {
                  value1 = 0;
                }
                String s = val ? 'ON' : 'OFF';
                databaseRef.reference().child('android').child('switch').set('${s}');
                databaseRef.reference().child('android').child('test').set('${value1}');
                databaseRef.reference().child('android').child('history').push().set('Switch is ${s}');
                databaseRef.reference().child('android').child('history').push().set('Fan speed is ${value1}');
              });
             },
              secondary : const Icon(Icons.home),
              )

              ),
           Align(
            alignment: Alignment(0.0,0.45),
            child: new SwitchListTile(
              title: new Text("MANUAL/AUTO"),
                value: _toggled1,
                onChanged: (bool val)
            {
                setState(() {
            _toggled1 = val;
                  if(val)
                  {
                      // String a = val ? 'AUTO' : 'MANUAL' ;
                      // databaseRef.reference().child('android').child('mode').set('${a}');
                      // databaseRef.reference().child('controller').child('test').once().then((DataSnapshot snapshot) {
                      //   double k=double.parse(snapshot.value);
                      //   if(k>0)
                      //   {
                      //     _toggled = true;
                      //   }
                      //   else
                      //   {
                      //     _toggled = false;
                      //   }
                      // });
                  }
                  else
                  {
                      databaseRef.reference().child('android').child('switch').set('${_toggled ? 'ON':'OFF'}');
                      databaseRef.reference().child('android').child('test').set('${value1}');
                  }
                  String a = val ? 'AUTO' : 'MANUAL' ;
                  databaseRef.reference().child('android').child('mode').set('${a}');
                  });
            },
                secondary : const Icon(Icons.accessibility),
            ),
          ),
          // Align(
          //   alignment: Alignment(0.4,0.85),
          //   child: new FloatingActionButton(
          //       child: new Icon(Icons.refresh),
          //       onPressed: ()
          //       {
          //           refresh();
          //       }),
          // ),
          Align(
              alignment: Alignment(0.0,0.85),
              child: new FloatingActionButton(
                onPressed: null,
                child : SpeedDial(

                    animatedIcon: AnimatedIcons.search_ellipsis,
                    animatedIconTheme: IconThemeData(size: 28.0),
                    backgroundColor: Colors.green,
                    closeManually:true,
                    visible: true,
                    renderOverlay: false,
                    overlayColor: Colors.black,
                    overlayOpacity: 0.5,
                    foregroundColor: Colors.black,
                    curve: Curves.bounceInOut,
                    children: [

                      SpeedDialChild(
                        child: Icon(Icons.thermostat_sharp, color: Colors.white),
                        // child: new Text('${temp}'),
                        backgroundColor: Colors.green,
                        onTap: () => print('Pressed Read Later'),
                        label: temp,
                        labelStyle:
                        TextStyle(fontWeight: FontWeight.w500, color: Colors.red),
                        labelBackgroundColor: Colors.black,
                      ),
                      SpeedDialChild(
                        child: Icon(Icons.local_fire_department_sharp, color: Colors.white),
                        // child: new Text('${hum}'),
                        backgroundColor: Colors.green,
                        onTap: () => print('Pressed Write'),
                        label: hum,
                        labelStyle:
                        TextStyle(fontWeight: FontWeight.w500, color: Colors.red),
                        labelBackgroundColor: Colors.black,
                      )
                    ]
                ),
              )
    ),
          Align(
            alignment: Alignment(0.0,-0.5),
            child: new Container(
              height: 50,
              child : new Text('${sgspd}')
              ),
            ),
        ]
              ),

    );
  }
}
