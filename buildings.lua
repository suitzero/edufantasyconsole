numbuildings = 5
heights={}
colors={}
for i=1,numbuildings do
    heights[i] = 100+math.random(100)
    colors[i] = math.random(20)
end

function draw_buildings()
    for i=1,numbuildings do
      bw =(256/numbuildings) fill_rect((i-1)*bw,heights[i],bw,256-heights[i],172+colors[i],172+colors[i],160+colors[i])

	  for j=0,2 do
		for k=0,10 do
			fill_rect((i-1)*bw+5+j*15,heights[i]+5+k*15,10,10,222,222,222)
		end 
	  end
    end
end

function _draw()
	fill_rect(0,0,256,256,153,217,234)
	draw_buildings()
	spr(1,100,30,16,12)
	spr(3,10,heights[1]-22,32,22)
	spr(3,220,heights[numbuildings]-22,32,22)
	spr(7,125,10,32,32)	
	write_text("hello",10,10,100,20)	
end



