import { StatusBar } from 'expo-status-bar';
import { StyleSheet, Text, View, SafeAreaView, Button } from 'react-native';
import React, { useState, useEffect } from 'react';




export default function App() {


    const [gasValues, setGasValues] = useState({});
  
    useEffect(() => {
        const interval = setInterval(async () => {
        const res = await fetch('http://192.168.43.138:3000/');
        const data = await res.json();
        setGasValues(data);
      }, 1000);
  
      return () => clearInterval(interval);
    }, []);



  console.log("App executed");
  const num_upper = Number(gasValues.upper);
  const num_lower = Number(gasValues.lower);    

  return (
    <SafeAreaView style={styles.container}>

      <View style={{
        backgroundColor: '#99C3D0',
        width: '90%',
        height: 130,
        borderRadius: 10,
        justifyContent: "center",
        alignItems: "center",
      }}> 
      <Text style={textStyles.baseText}>
      GAS MONITORING SYSTEM
      </Text>
        </View>  
      
        <View style={{
         top: 25,
         backgroundColor: '#99C3D0',
         width: '50%',
         height: 60,
         borderRadius: 10,
         justifyContent: "center",
         alignItems: "center",
         }}>
          <Button 
          color="black"
          title="ACTIVATE" onPress={() => console.log("Button - ACTIVATE tapped")} // zmienić czcionkę na mniejszą
          />
        </View>

        <View style={{
          top: 60,
          right: 90,
          backgroundColor: '#99C3D0',
          width: '40%',
          height: 50,
          borderRadius: 10,
          justifyContent: "center",
         alignItems: "center",
        }}>
          <Text style={textStyles.innerText}>
      Upper sensor:
      </Text>
          </View>
          
          <View style={{
          top: 10,
          left: 90,
          backgroundColor: 'white',
          width: '40%',
          height: 50,
          borderRadius: 10,
          justifyContent: "center",
          alignItems: "center",
        }}>
           <Text style={textStyles.innerText}>
      {gasValues.upper} ppm
      </Text>
          </View>

          <View style={{
          top: 50,
          right: 90,
          backgroundColor: '#99C3D0',
          width: '40%',
          height: 50,
          borderRadius: 10,
          justifyContent: "center",
         alignItems: "center",
        }}>
          <Text style={textStyles.innerText}>
      Lower sensor:
      </Text>
          </View>

          <View style={{
          left: 90,
          backgroundColor: 'white',
          width: '40%',
          height: 50,
          borderRadius: 10,
          justifyContent: "center",
          alignItems: "center",
        }}>
           <Text style={textStyles.innerText}>
           {gasValues.lower} ppm
      </Text>
          </View>


          <View style={{
          backgroundColor: '#5E8A03',
          top: 80,
          width: '90%',
          height: 50,
          borderRadius: 10,
          justifyContent: "center",
          alignItems: "center",
        }}>
           <Text style={textStyles.innerText}>
           Gas concentration in normal range.
          </Text>
          
          </View>

        

      </SafeAreaView>


  );
}


const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#235571',
    justifyContent: "top",
    alignItems: "center",
  
  },
});

const styles3 = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#99C3D0',
    justifyContent: "left",
    alignItems: "center",
  
  },
});


const textStyles = StyleSheet.create({
  baseText: {
    color: 'black',
    fontSize: 20
  },
  innerText: {
    color: 'black',
    fontSize: 17
  }
});

const styles2 = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#99C3D0',
    justifyContent: "center",
    alignItems: "center",
  
  },
});

