"POSITION_TEXTURE" : {
	pass default {

		vert = [[
			attribute vec2 a_texCoord;
			attribute vec4 a_position;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_Position = BW_MVPMatrix * a_position;
				v_texCoord = a_texCoord;
			}
		]]

		frag = [[
			precision mediump float;
			uniform sampler2D	Texture0;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_FragColor = texture2D(Texture0, v_texCoord);
			}
		]]
	}
}