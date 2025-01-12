# “ HOW HOT”--Temperature sensing assistance system
## Prototypes and ideas
Temperature and humidity are two important indicators in daily life. The change of temperature and humidity can lead to a change in people's body comfort. However, in daily life, people can intuitively see the changes in temperature and humidity, but can't observe the indoor or outdoor body comfort. The comfortable temperature (or humidity) plus the uncomfortable humidity (or temperature) will also lead to bad body feeling.</br>

Therefore, I hope to have an index that can combine temperature and humidity, and obtain an index through certain calculations, which more intuitively reflects the direct effect of temperature and humidity on human comfort. And convert the values into color information, to Chrono Lumina, so that people can more quickly and directly see the body comfort.</br>

The index I used is heat index(HI), which can combine temperature and humidity in a hot environment.</br>

![WechatIMG554](https://github.com/user-attachments/assets/47b50529-1935-4d24-ae9f-afdead9f2eef)

![5951736532971_ pic](https://github.com/user-attachments/assets/9bebd0be-2920-4d0a-89fa-eb76b168eab7)


## Device Introduction 
### Sensor
DHT22
### Functionality
1.When the heat index goes from low to high, the color of the light changes from Blue -&gt; Green -&gt; Yellow -&gt; Red. Thus, the body comfort can be intuitively seen.</br>
![5971736535023_ pic](https://github.com/user-attachments/assets/0e3926e4-d53b-4b74-add1-f5c750378472)

2.At the same time, for some people with color cognitive impairment (such as color weakness), the number and brightness of the lights can be used to display the value of heat index (for example, when the value of heat index is low, the lights will be less, and when the value of heat index is high, the lights will be more).</br>

![5981736535575_ pic](https://github.com/user-attachments/assets/915bc696-93f9-45ec-8842-c8dee5cc01e8)

## Structure

**first_versin**: The original version did not use heat index, only mixed temperature and humidity, tiled to RGB three channels, and did not have obvious color development.</br>
**second_versin**: Some temperature conditions were set to make the RGB color more obvious, but the actual values were not meaningful.</br>
**third_versin**: The heat index is used to control the color of the lamp, but only for a single light.</br>
**final_version_multi**: Changes in the value of the heat index are mapped to the number and brightness of lights, The overall 52 lights will be divided into four groups. As the value of HI increases, the color of the lights will not only change from Blue -&gt; Green -&gt;  Yellow -&gt;  Red, the number of lights on will also increase from left to right. And the brightness of the lights will increase.




