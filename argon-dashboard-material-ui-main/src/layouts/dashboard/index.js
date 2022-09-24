// @mui material components
import Grid from "@mui/material/Grid";
import React, { useEffect } from "react";
import axios from "axios";
import { useState } from "react";

// Argon Dashboard 2 MUI example components
import DashboardLayout from "examples/LayoutContainers/DashboardLayout";
import DetailedStatisticsCard from "examples/Cards/StatisticsCards/DetailedStatisticsCard";
import DefaultLineChart from "examples/Charts/LineCharts/DefaultLineChart";

function Default() {
  const [rpm, setRpm] = useState(null);
  const [voltage, setVoltage] = useState(null);
  const [S3, setS3] = useState(null);
  const [Vars, setVars] = useState(null);
  const [chartData, setChartData] = useState({});
  const [chartData2, setChartData2] = useState({});
  const [chartData3, setChartData3] = useState({});
  const [chartData4, setChartData4] = useState({});
  const [pwm, setPwm] = useState(100);
  const [isOff, setIsOff] = useState(1);
  const [buttonState, setButtonState] = useState(1);

  useEffect(() => {
    axios
      .get(
        "https://api.thingspeak.com/channels/1578993/fields/1.json?api_key=YEL5BFGCUHKVGIN9&results=30"
      )
      .then((response) => {
        const labels = [];
        const data = [];
        const data2 = [];
        const labels2 = [];
        const data3 = [];
        const labels3 = [];
        const data4 = [];
        const labels4 = [];
        axios
          .get(
            "https://api.thingspeak.com/channels/1578993/fields/2.json?api_key=YEL5BFGCUHKVGIN9&results=30"
          )
          .then((res) => {
            // response.data.feeds.map((val) => {
            //   if(val.field1 >= 0 && val.field1 < 150){
            //   labels.push(val.created_at);
            //   //push time of the field1 data to the data array
            //   data.push(val.field1);}
            //   // data.push(val.field1);}
            // });
            res.data.feeds.map((val) => {
              if(val.field2 >= 0 && val.field2 < 150){
              data2.push(val.field2);
              labels2.push(val.created_at);}
            });

            setChartData({
              labels: labels,
              datasets: [
                {
                  label: "Distance",
                  color: "info",
                  data: data,
                },
              ],

              // set min range of chart as 0 and step size as 500
              options: {
                scales: {
                  y: {
                    beginAtZero: true,
                    min: 0,
                    max: 5000,
                    ticks: {
                      stepSize: 500,
                    },
                  },
                },
              },
            });

            setChartData2({
              labels: labels2,
              datasets: [
                {
                  label: "Distance",
                  color: "info",
                  data: data2,
                },
              ],

              // set min range of chart as 0 and step size as 500
              options: {
                // make beginAtZero = true to start chart from 0
                scales: {
                  y: {
                    beginAtZero: true,
                    min: 0,
                    max: 5000,
                    ticks: {
                      stepSize: 500,
                    },
                  },
                },
              },
            });


          });

        axios
          .get(
            "https://api.thingspeak.com/channels/1578993/fields/3.json?api_key=YEL5BFGCUHKVGIN9&results=30"
          )
          .then((res) => {
            response.data.feeds.map((val) => {
              if(val.field1 >= 0 && val.field1 < 250){
              labels.push(val.created_at);
              data.push(val.field1);}
            });
            res.data.feeds.map((val) => {
              if(val.field3 >= 0 && val.field3 < 250){
              data3.push(val.field3);
              labels3.push(val.field3);}
            });

            setChartData({
              labels: labels,
              datasets: [
                {
                  label: "Distance",
                  color: "info",
                  data: data,
                },
              ],

              // set min range of chart as 0 and step size as 500
              options: {
                scales: {
                  y: {
                    beginAtZero: true,
                    min: 0,
                    max: 5000,
                    ticks: {
                      stepSize: 500,
                    },
                  },
                },
              },
            });

            setChartData3({
              labels: labels3,
              datasets: [
                {
                  label: "Distance",
                  color: "info",
                  data: data3,
                },
              ],

              // set min range of chart as 0 and step size as 500
              options: {
                // make beginAtZero = true to start chart from 0
                scales: {
                  y: {
                    beginAtZero: true,
                    min: 0,
                    max: 5000,
                    ticks: {
                      stepSize: 500,
                    },
                  },
                },
              },
            });

            // make a for loop for all elements in labels
            for (let i = 0; i < labels.length; i++) {
              if (labels[i] > 20) {
                labels4.push("Straight");
                data4.push(1);
              } else if (labels[i] <= 20 && labels2[i] > 10) {
                labels4.push("Left");
                data4.push(2);
              } else {
                labels4.push("Right");
                data4.push(3);
              }
            }

            setChartData4({
              labels: labels4,
              datasets: [
                {
                  label: "Distance",
                  color: "info",
                  data: data4,
                },
              ],

              // set min range of chart as 0 and step size as 500
              options: {
                // make beginAtZero = true to start chart from 0
                scales: {
                  y: {
                    beginAtZero: true,
                    min: 0,
                    max: 5000,
                    ticks: {
                      stepSize: 500,
                    },
                  },
                },
              },
            });
          });
      });

    axios
      .get(
        "https://api.thingspeak.com/channels/1578993/fields/1.json?api_key=YEL5BFGCUHKVGIN9&results=1"
      )
      .then((res) => {
        if(res.data.feeds[0].field1 >= 0 && res.data.feeds[0].field1 < 250){
        setRpm(res.data.feeds[0].field1);}
      });

    axios
      .get(
        "https://api.thingspeak.com/channels/1578993/fields/2.json?api_key=YEL5BFGCUHKVGIN9&results=1"
      )
      .then((res) => {
        if(res.data.feeds[0].field2 == 0){
        setVoltage("Straight");}
        else if(res.data.feeds[0].field2 == 1){
          setVoltage("Left");
        }
        else{
          setVoltage("Right");
        }
      });

    axios
      .get(
        "https://api.thingspeak.com/channels/1578993/fields/3.json?api_key=YEL5BFGCUHKVGIN9&results=1"
      )
      .then((res) => {
        setS3(res.data.feeds[0].field3);
      });

      axios.get('https://esw-onem2m.iiit.ac.in/~/in-cse/in-name/Team-20/Node-1/Data/la', {
            headers: {
                'X-M2M-Origin': 'ZrZ4zY:lC8jfN',
                'Content-type': 'application/json'
            },
            crossDomain: true,
        }).then(() => { })

    const labels4 = [];
    const data4 = [];

    if (rpm > 20) {
      labels4.push(1);
      data4.push(1);
    } else if (rpm <= 20 && voltage > 30) {
      labels4.push(2);
      data4.push(2);
    } else {
      labels4.push(3);
      data4.push(3);
    }

  }, []);

  const onChangeOff = (event) => {
    if (buttonState == 1){
      setIsOff(0)
    }
    else{
      setIsOff(1)
    }
  };

  useEffect(() => {
    console.log(buttonState)
    axios.get('https://api.thingspeak.com/update?api_key=0YOG8A5XJQ5D5F1A&field1=' + buttonState)
        .then(response => {
            console.log("value changed")
            console.log(response.status)
        })
  }, [isOff])


  return (
    <DashboardLayout>
      <Grid container spacing={3} mb={3}></Grid>
        <Grid item xs={12} md={6} lg={1}>
        <Grid item xs={12} md={6} lg={4}>
          <DetailedStatisticsCard
            title="Latest Distance from the Ultrasound Sensor"
            count={rpm}
            icon={{ color: "error", component: <i className="ni ni-world" /> }}
          />
        </Grid>
        <Grid item xs={12} md={6} lg={4}>
          <DetailedStatisticsCard
            title="Direction of the car"
            count={voltage}
            icon={{ color: "error", component: <i className="ni ni-world" /> }}
          />
        </Grid>
      </Grid>
      <Grid container spacing={3} mb={3}>
        <Grid item xs={12} md={6} lg={4}>
          
            </Grid>
            </Grid>
      <Grid>
        <Grid item xs={12} md={6} lg={4}>
          </Grid>
      </Grid>
      <Grid container spacing={4} mb={4}>
        {/* <button onClick={onChangeOff}>{ isOff ? 'On' : 'Off' }</button> */}
      </Grid>
      <Grid container spacing={3} mb={3}>
        <Grid item xs={12} md={6} lg={4}>
          
            </Grid>
            </Grid>
      <Grid></Grid>
      <Grid container spacing={4} mb={4}>
        <button onClick={() => {
          axios.get('https://api.thingspeak.com/update?api_key=0YOG8A5XJQ5D5F1A&field1=' + 1)
          .then(response => {
              console.log("turned on")
              console.log(response.status)
          })
        }}>{ 'Set on' }</button>
      </Grid>
      <Grid container spacing={4} mb={4}>
        <button onClick={() => {
          axios.get('https://api.thingspeak.com/update?api_key=0YOG8A5XJQ5D5F1A&field1=' + 0)
          .then(response => {
              console.log("turned off")
              console.log(response.status)
          })
        }}>{ 'Set off' }</button>
      </Grid>
      <Grid container spacing={3} mb={3}>
        <Grid item xs={20} md={20} lg={20}></Grid>
        <Grid item xs={12} lg={20}>
          <DefaultLineChart
            title="Ultrasound Sensor Readings"
            chart={chartData}
            beginAtZero
          />
        </Grid>
      </Grid>  
      <Grid container spacing={3} mb={3}>
        <Grid item xs={20} md={20} lg={20}></Grid>
        <Grid item xs={12} lg={20}>
          <DefaultLineChart
            title="Direction the robot turns (0-straight, 1-left, 2-right)"
            chart={chartData2}
            beginAtZero
          />
        </Grid>
      </Grid>      
    </DashboardLayout>
  );
}

export default Default;