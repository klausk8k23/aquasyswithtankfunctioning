package com.example.remote2;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.VoicemailContract;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.database.DatabaseReference;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Timer;
import java.util.TimerTask;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;
import org.jetbrains.annotations.NotNull;

public class MainActivity extends AppCompatActivity {
    FirebaseAuth auth;
    Button buttonLogout, buttonSched;
    TextView textView;
    FirebaseUser user;
    private OkHttpClient client = new OkHttpClient();
    private WebSocket webSocket;
    private WaveProgressBar waveProgressBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getSupportActionBar().hide();
        setContentView(R.layout.activity_main);

        auth = FirebaseAuth.getInstance();

        buttonSched = findViewById(R.id.btnSched);
        buttonSched.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), SetSchedule.class);
                startActivity(intent);
                finish();
            }
        });

        buttonLogout = findViewById(R.id.btnLogout);
        textView = findViewById(R.id.user);
        user = auth.getCurrentUser();
        if (user == null) {
            Intent intent = new Intent(getApplicationContext(), Login.class);
            startActivity(intent);
            finish();
        } else {
            textView.setText(user.getEmail());
        }

        // Initialize OkHttpClient
        client = new OkHttpClient();

        // Initialize WaveProgressBar
        waveProgressBar = findViewById(R.id.waveProgressbar);

        // Set up and start the WebSocket connection
        Request request = new Request.Builder().url("ws:192.168.1.7:81").build();

        webSocket = client.newWebSocket(request, new WebSocketListener() {
            @Override
            public void onMessage(@NotNull WebSocket webSocket, @NotNull String text) {
                // Handle received WebSocket message (percentage and distance)
                String[] parts = text.split(",");
                if (parts.length == 2) {
                    String percentage = parts[0];
                    String distance = parts[1];

                    // Update the WaveProgressBar with the received percentage
                    int percentageValue = Integer.parseInt(percentage);
                    updateWaveProgressBar(percentageValue);
                }
            }
        });
    }

    // Method to update the WaveProgressBar
    // Method to update the UI element with percentage value
    private void updateWaveProgressBar(int percentage) {
        // Ensure the percentage is within valid bounds (0-100)
        percentage = Math.min(100, Math.max(0, percentage));

        // Update the UI element (e.g., progress bar)
        int finalPercentage = percentage;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                waveProgressBar.setProgress(finalPercentage);
            }
        });
    }}
