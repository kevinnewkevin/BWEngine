"POSITION_TEXTURE" : {
	pass default {

		vert = [[
			attribute vec2 a_texCoord;
			attribute vec4 a_position;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_Position = MVPMatrix * a_position;
				v_texCoord = a_texCoord;
			}
		]]

		frag = [[
			precision mediump float;
			varying vec2 v_texCoord;
			void main() 
			{
				gl_FragColor = texture2D(Texture0, v_texCoord);
			}
		]]
	}
}