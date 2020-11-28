=from machine import Pin

# 'safe' pins according to pinout at: https://raw.githubusercontent.com/LilyGO/LILYGO-T-Energy/master/T18v3.0.jpg

#BOOT button side
p15 = Pin(15, Pin.IN, Pin.PULL_UP)
p2 = Pin(2, Pin.IN, Pin.PULL_UP)
p4 = Pin(4, Pin.IN, Pin.PULL_UP)
p5 = Pin(5, Pin.IN, Pin.PULL_UP)
p18 = Pin(18, Pin.IN, Pin.PULL_UP) # this is listed on pinout as 'GPIO22' which I assume is an error
p19 = Pin(19, Pin.IN, Pin.PULL_UP)
p21 = Pin(21, Pin.IN, Pin.PULL_UP)
p23 = Pin(23, Pin.IN, Pin.PULL_UP)

bootPins = {
  "GPIO 15": p15.value(), 
  "GPIO 2": p2.value(), 
  "GPIO 4": p4.value(), 
  "GPIO 5": p5.value(), 
  "GPIO 18": p18.value(), 
  "GPIO 19": p19.value(), 
  "GPIO 21": p21.value(), 
  "GPIO 23": p23.value()
}


# EN button side
p13 = Pin(13, Pin.IN, Pin.PULL_UP)
p12 = Pin(12, Pin.IN, Pin.PULL_UP)
p14 = Pin(14, Pin.IN, Pin.PULL_UP)
p27 = Pin(27, Pin.IN, Pin.PULL_UP)
p26 = Pin(26, Pin.IN, Pin.PULL_UP)
p25 = Pin(25, Pin.IN, Pin.PULL_UP)
p35 = Pin(35, Pin.IN, Pin.PULL_UP)
#p34 = Pin(34, Pin.IN, Pin.PULL_UP)

enPins = {
  "GPIO 13": p13.value(), 
  "GPIO 12": p12.value(), 
  "GPIO 14": p14.value(), 
  "GPIO 27": p27.value(), 
  "GPIO 26": p26.value(), 
  "GPIO 25": p25.value(), 
  "GPIO 35": p35.value(), 
  #"GPIO 34": p34.value()
}

# probably shouldnt use...

#p33 = Pin(33, Pin.IN, Pin.PULL_UP)
#p32 = Pin(32, Pin.IN, Pin.PULL_UP)
#p39 = Pin(39, Pin.IN, Pin.PULL_UP)
#p36 = Pin(36, Pin.IN, Pin.PULL_UP)

#p3 = Pin(3, Pin.IN, Pin.PULL_UP) # RX
#p1= Pin(1, Pin.IN, Pin.PULL_UP) # TX
# p22 = Pin(22, Pin.IN, Pin.PULL_UP) # SCL


#p0 = Pin(3, Pin.IN, Pin.PULL_UP)

#p6 = Pin(6, Pin.IN, Pin.PULL_UP)
#p7 = Pin(7, Pin.IN, Pin.PULL_UP)
#p8 = Pin(8, Pin.IN, Pin.PULL_UP)
#p9 = Pin(9, Pin.IN, Pin.PULL_UP)
#p10 = Pin(10, Pin.IN, Pin.PULL_UP)
#p11 = Pin(11, Pin.IN, Pin.PULL_UP)
#p16 = Pin(16, Pin.IN, Pin.PULL_UP)
#p17 = Pin(17, Pin.IN, Pin.PULL_UP)
#p20 = Pin(20, Pin.IN, Pin.PULL_UP)
#p24= Pin(24, Pin.IN, Pin.PULL_UP)
#p28 = Pin(28, Pin.IN, Pin.PULL_UP)
#p29 = Pin(29, Pin.IN, Pin.PULL_UP)
#p30 = Pin(30, Pin.IN, Pin.PULL_UP)
#p31 = Pin(31, Pin.IN, Pin.PULL_UP)
#p37 = Pin(37, Pin.IN, Pin.PULL_UP)
#p38 = Pin(38, Pin.IN, Pin.PULL_UP)

print("\n== Pins on BOOT side of board == \n")
print("\n".join("{!r}: {!r},".format(k, v) for k, v in bootPins.items()))
print("\n== Pins on EN side of board == \n")
print("\n".join("{!r}: {!r},".format(k, v) for k, v in enPins.items()))

