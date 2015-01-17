"default" : {
	pass default {

		vert = [[
			attribute vec2 vTexCoord;
			attribute vec4 vPosition;
			uniform mat4 MVP;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_Position = MVP*vPosition;
				v_texCoord = vTexCoord;
			}
		]]

		frag = [[
			precision mediump float;
			uniform sampler2D texture1;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_FragColor = texture2D(texture1, v_texCoord);
			}
		]]
	}
}