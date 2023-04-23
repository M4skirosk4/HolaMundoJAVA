package Pomodoro;
import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.Timer;

public class InterfazCronometro extends JFrame {

  private Timer temporizador; // objeto Timer para el cronómetro
  private int segundos = 5; // segundos transcurridos
  private JLabel etiquetaTiempo; // etiqueta para mostrar el tiempo transcurrido
  private JButton botonIniciar; // botón para iniciar el cronómetro
  private JButton botonDetener; // botón para detener el cronómetro
  private JButton botonArribaDerecha; // botón en la esquina superior derecha
  private boolean modo = false; //Variable para contar el número total de pomodoros
 
  public InterfazCronometro() {
   
    // crea un panel para el contador de tiempo
    JPanel panelCronometro = new JPanel();
    etiquetaTiempo = new JLabel("Tiempo: " + segundos + " segundos");
    etiquetaTiempo.setFont(new Font("Arial", Font.PLAIN, 24));
    panelCronometro.add(etiquetaTiempo);

    // agrega un botón para detener el cronómetro
    botonDetener = new JButton("Detener");
    botonDetener.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        detenerCronometro();
        botonIniciar.setEnabled(true);
        botonDetener.setEnabled(false);
      }
    });
    panelCronometro.add(botonDetener);

    // agrega un botón para iniciar el cronómetro
    botonIniciar = new JButton("Iniciar");
    botonIniciar.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        iniciarCronometro();
        botonIniciar.setEnabled(false);
        botonDetener.setEnabled(true);
        
      }
    });
    panelCronometro.add(botonIniciar);

    // agrega el panel al contenedor principal
    add(panelCronometro);

    
    // crea un panel para el botón en la esquina superior derecha
    JPanel panelBoton = new JPanel(new BorderLayout());
    botonArribaDerecha = new JButton("Ajustes");
    panelBoton.add(botonArribaDerecha, BorderLayout.EAST);
    add(panelBoton, BorderLayout.NORTH);

    // configura la ventana
    setSize(300, 200);
    setVisible(true);
    setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  


    
  }
     

  public void iniciarCronometro() {
    // crea un nuevo temporizador y lo inicia
    temporizador = new Timer(1000, new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        segundos--;
        etiquetaTiempo.setText("Tiempo: " + segundos + " segundos");
      }
    });
    temporizador.start();
  }
  public void detenerCronometro() {
    // detiene el temporizador y reinicia los segundos
    temporizador.stop();
    segundos = 5;
    etiquetaTiempo.setText("Tiempo: 0 segundos");
  }
}