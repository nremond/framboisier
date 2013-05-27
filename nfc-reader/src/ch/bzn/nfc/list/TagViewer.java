package ch.bzn.nfc.list;

import java.util.HashSet;
import java.util.Set;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.Intent;
import android.media.AudioManager;
import android.media.SoundPool;
import android.media.SoundPool.OnLoadCompleteListener;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewPropertyAnimator;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;
import android.widget.TextView;

/**
 * An {@link Activity} which handles a broadcast of a new tag that the device
 * just discovered.
 */
public class TagViewer extends Activity {

	private RelativeLayout mTagContent;

	private NfcAdapter mAdapter;
	private PendingIntent mPendingIntent;
	private TextView textViewTag;
	private ViewPropertyAnimator animate;
	private AlertDialog mDialog;
	private SoundPool soundPool;
	private TextView itemsTextView;
	private Set<Long> idsChecked;

	private int idSoundRight;
	private int idSoundWrong;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		ActionBar actionBar = getActionBar();
		itemsTextView = new TextView(actionBar.getThemedContext());
		itemsTextView.setBackgroundColor(0x22EE22);
		actionBar.setCustomView(itemsTextView);
		actionBar.setDisplayOptions(ActionBar.DISPLAY_USE_LOGO|ActionBar.DISPLAY_SHOW_HOME|ActionBar.DISPLAY_SHOW_TITLE|ActionBar.DISPLAY_SHOW_CUSTOM);
		setContentView(R.layout.tag_viewer);
		mTagContent = (RelativeLayout) findViewById(R.id.list);
		resolveIntent(getIntent());

		mDialog = new AlertDialog.Builder(this).setNeutralButton("Ok", null)
				.create();

		mAdapter = NfcAdapter.getDefaultAdapter(this);
		if (mAdapter == null) {
			showMessage(R.string.error, R.string.no_nfc);
		}
		mPendingIntent = PendingIntent.getActivity(this, 0, new Intent(this,
				getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
		
		resetIdsChecked();
		//handle click on reset button
		((Button)findViewById(R.id.resetButton)).setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				resetIdsChecked();
			}
		});
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundPool.setOnLoadCompleteListener(new OnLoadCompleteListener() {
			@Override
			public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
			}
		});
		idSoundRight = soundPool.load(this, R.raw.checkright, 1);
		idSoundWrong = soundPool.load(this, R.raw.checkwrong, 1);
	}

	protected void resetIdsChecked() {
		idsChecked = new HashSet<Long>();
		itemsTextView.setText("0");
	}

	private void showMessage(int title, int message) {
		mDialog.setTitle(title);
		mDialog.setMessage(getText(message));
		mDialog.show();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		if (mAdapter != null) {
			if (!mAdapter.isEnabled()) {
				showMessage(R.string.error, R.string.nfc_disabled);
			}
			mAdapter.enableForegroundDispatch(this, mPendingIntent, null, null);
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		if (mAdapter != null) {
			mAdapter.disableForegroundDispatch(this);
		}
	}

	private void resolveIntent(Intent intent) {
		String action = intent.getAction();
		if (NfcAdapter.ACTION_TAG_DISCOVERED.equals(action)
				|| NfcAdapter.ACTION_TECH_DISCOVERED.equals(action)
				|| NfcAdapter.ACTION_NDEF_DISCOVERED.equals(action)) {
			long id = getDec(intent.getByteArrayExtra(NfcAdapter.EXTRA_ID));
			StringBuilder sb = new StringBuilder("Tag ID read : ");
			sb.append(id);
			if (textViewTag == null) {
				instantiateViewTagAndAnimate();
			}
			textViewTag.setText(sb.toString());
			animate.alpha(1);
			int soundToPlay = idSoundWrong;
			if(!idsChecked.contains(id)){
				idsChecked.add(id);
				itemsTextView.setText(""+idsChecked.size());
				soundToPlay = idSoundRight;
			}
			soundPool.play(soundToPlay, 1, 1, 1,0,1f);
			
		}
	}

	private void instantiateViewTagAndAnimate() {
		textViewTag = new TextView(this);
		LayoutParams params = new LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT);
		params.addRule(RelativeLayout.BELOW, R.id.intro);
		textViewTag.setLayoutParams(params);
		textViewTag.setAlpha(0);
		animate = textViewTag.animate();
		animate.setDuration(1000L);
		AnimatorListener listener = new AnimatorListener() {

			@Override
			public void onAnimationStart(Animator animation) {
			}

			@Override
			public void onAnimationRepeat(Animator animation) {
			}

			@Override
			public void onAnimationEnd(Animator animation) {
				animate.alpha(0);
			}

			@Override
			public void onAnimationCancel(Animator animation) {
			}
		};
		animate.setListener(listener);
		mTagContent.addView(textViewTag);
	}

	private long getDec(byte[] bytes) {
		long result = 0;
		long factor = 1;
		for (int i = 0; i < bytes.length; ++i) {
			long value = bytes[i] & 0xffl;
			result += value * factor;
			factor *= 256l;
		}
		return result;
	}

	@Override
	public void onNewIntent(Intent intent) {
		setIntent(intent);
		resolveIntent(intent);
	}
}
